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
 * @date: 2023-11-15 15:01:49
 * @file: XbftEngine.h
 */

#ifndef __XBFT_ENGINE_H__
#define __XBFT_ENGINE_H__

#include "ConsEngine.h"
#include <memory>
#include <string>

namespace xbft {

class XbftConsensus;
class RoundRobinPaceMaker;

class XbftEngine : public BftEngine {
public:
    XbftEngine(std::shared_ptr<NetInterface> p_net, std::shared_ptr<ValueDealInterface> p_valueDeal,
        std::shared_ptr<KeyToolInterface> p_keyTool);
    ~XbftEngine();

public:
    std::string GetEngineName();
    std::string GetEngineVersion();
    bool StartEngine(std::shared_ptr<NodeInfoInterface> p_nodeInfo);

public:
    void Rotate();
    bool Propose(std::shared_ptr<ConsData> p_consData);
    void Recv(const std::string &cr_consMsg, std::shared_ptr<KeyToolInterface> p_keyTool);

    bool IsLeader();
    std::string GetLatestProof();
    int64_t GetViewNumber();
    bool IsViewActive();
    size_t GetQuorumSize();

private:
    std::shared_ptr<XbftConsensus> mp_consensus;
    std::shared_ptr<RoundRobinPaceMaker> mp_paceMaker;
};

}  // namespace xbft

#endif
