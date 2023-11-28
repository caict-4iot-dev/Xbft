/*
 * © COPYRIGHT 2022 Corporation CAICT All rights reserved.
 *  http://www.caict.ac.cn
 *  https://bitfactory.cn
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @date: 2023-11-23 11:23:22
 * @file: Network.cpp
 */

#include "Network.h"
#include "ConsEngine.h"
#include "ExitHandler.h"
#include "Logger.h"
#include "Strings.h"
#include "chain.pb.h"
#include "utils/Strings.h"
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>


namespace net {
Network::Network() : m_nodeSocket(-1), mp_sendThread(nullptr), mp_recvThread(nullptr) {}

Network::~Network() {}

bool Network::Initialize() {
    // 创建socket
    m_nodeSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_nodeSocket == -1) {
        LOG_ERROR("Network Failed to create socket...");
        return false;
    }

    // 设置本地地址
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(common::NetConfig::ms_port);  // 设置本地端口
    localAddr.sin_addr.s_addr = INADDR_ANY;                  // 设置本地IP地址

    // 绑定socket到本地地址
    if (bind(m_nodeSocket, (sockaddr *)&localAddr, sizeof(localAddr)) == -1) {
        LOG_ERROR("Network bind socket failed...");
        close(m_nodeSocket);
        return false;
    }

    mp_recvThread = std::make_shared<std::thread>(&Network::recvMsg, this);
    mp_sendThread = std::make_shared<std::thread>(&Network::sendMsg, this);
    buildAddressAndIp();

    sendCons = net::SendConsData;
    sendSync = net::SendSyncData;

    LOG_INFO("Network Initialize [OK]...");
    return true;
}

bool Network::Exit() {
    LOG_INFO("Network::Exit");
    // 关闭socket
    close(m_nodeSocket);

    mp_recvThread->join();
    mp_sendThread->join();

    LOG_INFO("Network stop [OK]");
    return true;
}

void Network::SendMsg(
    const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value) {
    common::Msg msgEvent;
    msgEvent.crValue = std::move(cr_value);
    msgEvent.crDest = std::move(cr_dest);

    LOG_INFO("sendMsg from:%s", cr_from.c_str());

    // LOG_INFO("SendMsg data %s, size %d, ", cr_value, cr_value.size());
    m_sendMsgQueue.Push(msgEvent);
}

common::EventQueue<std::string> &Network::GetMsgQueue() {
    return m_recvMsgQueue;
}

common::EventQueue<common::SyncMsg> &Network::GetSyncMsgQueue() {
    return m_recvSyncMsgQueue;
}

void Network::buildAddressAndIp() {
    m_bootnode.clear();
    m_bootsock.clear();

    for (auto node : common::NetConfig::ms_bootnode) {
        auto address = node["address"];
        auto ipstr = node["consensus_network"];
        m_bootnode[ipstr] = address;

        common::StringVector ip_array = common::String::Strtok(ipstr, ':');
        if (ip_array.size() <= 1) {
            LOG_ERROR("[net] [bootnode] [consensus_network] config error...");
            continue;
        }

        sockaddr_in peerAddr;
        peerAddr.sin_family = AF_INET;
        peerAddr.sin_port = htons(common::String::ToUint(ip_array[1]));  // 设置对方端口
        inet_pton(AF_INET, ip_array[0].c_str(), &peerAddr.sin_addr);     // 设置对方IP地址

        m_bootsock[address] = peerAddr;
    }
}

std::string Network::sockAddrToStr(const sockaddr_in &cr_addr) {
    char ip[INET_ADDRSTRLEN];
    char port[10];
    inet_ntop(AF_INET, &(cr_addr.sin_addr), ip, INET_ADDRSTRLEN);
    sprintf(port, "%d", ntohs(cr_addr.sin_port));

    std::string fromStr = std::string(ip) + ":" + std::string(port);
    return fromStr;
}

void Network::sendMsg() {
    while (!common::ExitHandler::GetExitFlag()) {
        common::Msg msgEvent;
        if (!m_sendMsgQueue.TryPop(std::chrono::milliseconds(100), msgEvent)) {
            continue;
        }

        // 目的地址转化为ip
        std::vector<sockaddr_in> sockaddrList;
        for (auto dest : msgEvent.crDest) {
            if (m_bootsock.count(dest) != 0) {
                sockaddrList.push_back(std::move(m_bootsock[dest]));
            }
        }

        // 发送数据到对方节点
        std::string &crValue = msgEvent.crValue;
        LOG_INFO("send data size %ld, ", crValue.size());
        for (auto peerAddr : sockaddrList) {
            sendto(m_nodeSocket, crValue.c_str(), crValue.size(), 0, (sockaddr *)&peerAddr, sizeof(peerAddr));
        }
    }
}

void Network::recvMsg() {
    while (!common::ExitHandler::GetExitFlag()) {
        sockaddr_in fromAddr;
        socklen_t fromAddrLen = sizeof(fromAddr);
        char recvBuf[50 * 1024] = {0};
        memset(recvBuf, 0, 50 * 1024);
        int bytesReceived =
            recvfrom(m_nodeSocket, recvBuf, 50 * 1024, MSG_DONTWAIT, (sockaddr *)&fromAddr, &fromAddrLen);
        if (bytesReceived > -1) {
            std::string recvData = std::string(recvBuf, bytesReceived);
            LOG_INFO("recvMsg size %ld recvData size:%ld", bytesReceived, recvData.size());
            protocol::Message message;
            message.ParseFromString(recvBuf);
            if (message.type() == protocol::MESSAGE_TYPE::MSG_CONSENSUS) {
                m_recvMsgQueue.Push(utils::String::HexStringToBin(message.data()));
            } else if (message.type() == protocol::MESSAGE_TYPE::MSG_SYNC) {
                auto fromStr = sockAddrToStr(fromAddr);
                if (m_bootnode.count(fromStr) != 0) {
                    common::SyncMsg msg;
                    msg.request = message.request();
                    msg.crValue = utils::String::HexStringToBin(message.data());
                    msg.crDest = m_bootnode[fromStr];
                    m_recvSyncMsgQueue.Push(msg);
                } else {
                    LOG_ERROR("Network::recvMsg Unknow Sync infomation, fromStr:%s", fromStr.c_str());
                }

            } else {
                LOG_INFO("Network recvMsg Invalid message");
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void SendConsData(const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value) {
    LOG_INFO("Net Send cons data size %ld", cr_value.size());
    protocol::Message message;
    message.set_type(protocol::MESSAGE_TYPE::MSG_CONSENSUS);
    message.set_data(utils::String::BinToHexString(cr_value));
    Network::Instance().SendMsg(cr_from, cr_dest, message.SerializeAsString());
}

void SendSyncData(
    const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value, bool request) {
    LOG_INFO("Net Send sync data size %ld", cr_value.size());
    protocol::Message message;
    message.set_type(protocol::MESSAGE_TYPE::MSG_SYNC);
    message.set_request(request);
    message.set_data(utils::String::BinToHexString(cr_value));
    Network::Instance().SendMsg(cr_from, cr_dest, message.SerializeAsString());
}
}  // namespace net
