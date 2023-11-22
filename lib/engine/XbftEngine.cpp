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
 * @date: 2023-11-15 15:02:02
 * @file: XbftEngine.cpp
 */

#include "XbftEngine.h"
#include "XbftConsensus.h"
#include "XbftLiveness.h"

namespace xbft {

XbftEngine::XbftEngine(std::shared_ptr<NetInterface> p_net, std::shared_ptr<ValueDealInterface> p_valueDeal,
    std::shared_ptr<KeyToolInterface> p_keyTool) {
    mp_consensus = std::make_shared<XbftConsensus>(p_net, p_valueDeal, p_keyTool);
    mp_paceMaker = std::make_shared<RoundRobinPaceMaker>(p_net, p_keyTool, mp_consensus);
}

XbftEngine::~XbftEngine() {
    mp_paceMaker->Exit();
    mp_consensus->Exit();
}

std::string XbftEngine::GetEngineName() {
    return "Xbft Engine";
}

std::string XbftEngine::GetEngineVersion() {
    return "1.0.0";
}

bool XbftEngine::StartEngine(std::shared_ptr<NodeInfoInterface> p_nodeInfo) {
    if (!mp_consensus->Initialize(p_nodeInfo->GetKeyInfo(), p_nodeInfo->GetValidators(),
            p_nodeInfo->GetLastViewNumber(), p_nodeInfo->GetLastProof())) {
        return false;
    }

    if (!mp_paceMaker->Initialize(p_nodeInfo->GetSendMsgInterval())) {
        return false;
    }

    return true;
}

void XbftEngine::Rotate() {
    mp_paceMaker->Rotate();
}

bool XbftEngine::Propose(std::shared_ptr<ConsData> p_consData) {
    return mp_consensus->Propose(p_consData);
}

void XbftEngine::Recv(const std::string &cr_consMsg, std::shared_ptr<KeyToolInterface> p_keyTool) {
    protocol::XbftEnv xbftEnv;
    xbftEnv.ParseFromString(cr_consMsg);
    XbftMsgPointer ptr = std::make_shared<XbftMsg>(xbftEnv, p_keyTool);
    if (ptr->GetType() == protocol::XBFT_TYPE_NEWVIEW) {
        mp_paceMaker->OnRecv(ptr);
    } else {
        mp_consensus->OnRecv(ptr);
    }
}

bool XbftEngine::IsLeader() {
    return mp_consensus->IsLeader();
}

std::string XbftEngine::GetLatestProof() {
    return mp_consensus->GetLastProof();
}

int64_t XbftEngine::GetViewNumber() {
    return mp_consensus->GetViewNumber();
}

bool XbftEngine::IsViewActive() {
    return mp_consensus->IsViewActive();
}

size_t XbftEngine::GetQuorumSize() {
    return mp_consensus->GetQuorumSize();
}


}  // namespace xbft