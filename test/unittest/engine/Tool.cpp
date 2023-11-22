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
 * @author: maxufeng@caict.ac.cn
 * @date: 2023-11-21 17:03:39
 * @file: Tool.cpp
 */

#include "Tool.h"

std::vector<XbftEngineMsg> Tool::FetchMsgAndClear() {
    std::lock_guard<std::mutex> guard(g_msgLock);
    std::vector<XbftEngineMsg> temp = g_msgs;
    g_msgs.clear();
    return temp;
}

std::vector<XbftEngineValueCommited> Tool::FetchCommitedAndClear() {
    std::lock_guard<std::mutex> guard(g_msgLock);
    std::vector<XbftEngineValueCommited> temp = g_valueCommited;
    g_valueCommited.clear();
    return temp;
}

std::shared_ptr<xbft::NetInterface> Tool::CreateNet() {
    auto net = std::make_shared<xbft::NetInterface>();
    net->m_sendMsg = toolMock::sendMsg;

    return net;
}

std::shared_ptr<xbft::KeyToolInterface> Tool::CreateKeyTool() {
    auto keyTool = std::make_shared<xbft::KeyToolInterface>();
    keyTool->m_verify = toolMock::verify;
    keyTool->m_publicKeyToAddr = toolMock::pubKeyToAddress;
    keyTool->m_createConsData = toolMock::createConsData;

    return keyTool;
}

std::shared_ptr<xbft::ValueDealInterface> Tool::CreateValueDeal() {
    auto valueDeal = std::make_shared<xbft::ValueDealInterface>();
    valueDeal->m_checkValue = toolMock::checkValue;
    valueDeal->m_valueCommited = toolMock::valueCommited;

    return valueDeal;
}

std::shared_ptr<tmpNodeInfo> Tool::CreateNodeInfo(std::shared_ptr<xbft::KeyInterface> p_key, int64_t interVal) {
    auto nodeInfo = std::make_shared<tmpNodeInfo>();
    nodeInfo->mp_key = p_key;
    nodeInfo->m_interval = interVal;

    return nodeInfo;
}

std::shared_ptr<Key> Tool::CreateKeyInfo(const std::string &index) {
    auto keyInfo = std::make_shared<Key>();
    std::string pri = "pri-" + index;
    std::string pub = "pub-" + index;
    std::string address = "address-" + index;
    keyInfo->SetPri(pri);
    keyInfo->SetPub(pub);
    keyInfo->SetAdd(address);

    return keyInfo;
}
