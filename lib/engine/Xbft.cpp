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
 * @date: 2023-11-15 17:15:33
 * @file: Xbft.cpp
 */

#include "ConsEngine.h"
#include "XbftEngine.h"
#include <memory>

namespace xbft {
std::shared_ptr<BftEngine> CreateXbftEngine(std::shared_ptr<NetInterface> p_net,
    std::shared_ptr<ValueDealInterface> p_valueDeal, std::shared_ptr<NodeInfoInterface> p_nodeInfo,
    std::shared_ptr<KeyToolInterface> p_keyTool) {
    auto engine = std::make_shared<XbftEngine>(p_net, p_valueDeal, p_keyTool);
    if (!engine->StartEngine(p_nodeInfo)) {
        return nullptr;
    }
    return engine;
}

void Rotate(std::shared_ptr<BftEngine> p_engine) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    xbftEngine->Rotate();
}

bool Propose(std::shared_ptr<BftEngine> p_engine, std::shared_ptr<ConsData> p_consData) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    return xbftEngine->Propose(p_consData);
}

void Recv(
    std::shared_ptr<BftEngine> p_engine, const std::string &cr_consMsg, std::shared_ptr<KeyToolInterface> p_keyTool) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    xbftEngine->Recv(cr_consMsg, p_keyTool);
}

bool IsLeader(std::shared_ptr<BftEngine> p_engine) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    return xbftEngine->IsLeader();
}

std::string GetLatestProof(std::shared_ptr<BftEngine> p_engine) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    return xbftEngine->GetLatestProof();
}

int64_t GetViewNumber(std::shared_ptr<BftEngine> p_engine) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    return xbftEngine->GetViewNumber();
}

bool IsViewActive(std::shared_ptr<BftEngine> p_engine) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    return xbftEngine->IsViewActive();
}

size_t GetQuorumSize(std::shared_ptr<BftEngine> p_engine) {
    std::shared_ptr<XbftEngine> xbftEngine = std::static_pointer_cast<XbftEngine>(p_engine);
    return xbftEngine->GetQuorumSize();
}

}  // namespace xbft