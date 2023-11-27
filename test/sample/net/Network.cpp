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
#include "Logger.h"
#include "Strings.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include "utils/Strings.h"

namespace net {
Network::Network()
    : m_nodeSocket(-1), mp_sendThread(nullptr), mp_recvThread(nullptr), mp_recvBuf(nullptr), m_isExit(false) {}

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

    mp_recvBuf = new char[50 * 1024];
    mp_recvThread = new std::thread(&Network::recvMsg, this);
    mp_sendThread = new std::thread(&Network::sendMsg, this);


    mp_net = std::make_shared<xbft::NetInterface>();
    if (mp_net == nullptr) {
        LOG_ERROR("Network build mp_net nullptr");
        return false;
    }
    mp_net->m_sendMsg = net::Send;

    LOG_INFO("Network Initialize [OK]...");
    return true;
}

bool Network::Exit() {
    m_isExit = true;
    LOG_INFO("Network::Exit");
    // 关闭socket
    close(m_nodeSocket);

    if (mp_recvThread != nullptr) {
        mp_recvThread->join();
        delete mp_recvThread;
        mp_recvThread = nullptr;
    }

    if (mp_sendThread != nullptr) {
        mp_sendThread->join();
        delete mp_sendThread;
        mp_sendThread = nullptr;
    }

    if (mp_recvBuf != nullptr) {
        delete[] mp_recvBuf;
        mp_recvBuf = nullptr;
    }

    LOG_INFO("Network stop [OK]");
    return true;
}

void Network::SendMsg(
    const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value) {
    Msg msgEvent;
    msgEvent.crValue = std::move(cr_value);
    msgEvent.crDest = std::move(cr_dest);

    LOG_INFO("sendMsg from:%s", cr_from.c_str());

    // LOG_INFO("SendMsg data %s, size %d, ", cr_value, cr_value.size());
    m_sendMsgQueue.Push(msgEvent);
}

common::EventQueue<std::string> &Network::GetMsgQueue() {
    return m_recvMsgQueue;
}

void Network::sendMsg() {
    while (!m_isExit) {
        Msg msgEvent;
        if (!m_sendMsgQueue.TryPop(std::chrono::milliseconds(100), msgEvent)) {
            continue;
        }

        // 目的地址转化为ip
        std::vector<std::string> ipDest;
        bool found = false;
        for (auto dest : msgEvent.crDest) {
            for (auto node : common::NetConfig::ms_bootnode) {
                if (node["address"] == dest) {
                    ipDest.push_back(node["consensus_network"]);
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            LOG_ERROR("[net] [bootnode] [address] config error...");
        }

        // 设置对方地址
        std::vector<sockaddr_in> sockaddrList;
        for (size_t i = 0; i < ipDest.size(); i++) {
            LOG_INFO("send peerAddr:%s", ipDest[i].c_str());

            common::StringVector ip_array = common::String::Strtok(ipDest[i], ':');
            if (ip_array.size() <= 1) {
                LOG_ERROR("[net] [bootnode] [consensus_network] config error...");
                continue;
            }

            sockaddr_in peerAddr;
            peerAddr.sin_family = AF_INET;
            peerAddr.sin_port = htons(common::String::ToUint(ip_array[1]));  // 设置对方端口
            inet_pton(AF_INET, ip_array[0].c_str(), &peerAddr.sin_addr);     // 设置对方IP地址

            sockaddrList.push_back(std::move(peerAddr));
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
    while (!m_isExit) {
        sockaddr_in fromAddr;
        socklen_t fromAddrLen = sizeof(fromAddr);
        char recvBuf[50 * 1024] = {0};
        memset(recvBuf, 0, 50 * 1024);
        int bytesReceived =
            recvfrom(m_nodeSocket, recvBuf, 50 * 1024, MSG_DONTWAIT, (sockaddr *)&fromAddr, &fromAddrLen);
        if (bytesReceived > -1) {
            std::string recvData = std::string(recvBuf, bytesReceived);
            LOG_INFO("recvMsg size %ld recvData size:%ld", bytesReceived, recvData.size());
            m_recvMsgQueue.Push(recvData);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Send(const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value) {
    LOG_INFO("Net Send data size %ld", cr_value.size());
    Network::Instance().SendMsg(cr_from, cr_dest, cr_value);
}
}  // namespace net
