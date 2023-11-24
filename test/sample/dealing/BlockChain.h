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
 * @date: 2023-11-22 17:42:27
 * @file: BlockChain.h
 */

#ifndef __BLOCK_CHAIN_H__
#define __BLOCK_CHAIN_H__

#include "ConsEngine.h"
#include "Singleton.h"
#include "Timer.h"
#include <string>


namespace dealing {
class NodeInfo;
class BlockChain : public common::Singleton<BlockChain> {
    friend class common::Singleton<BlockChain>;

public:
    BlockChain();
    ~BlockChain() = default;
    bool Initialize(std::shared_ptr<xbft::NetInterface> p_net);
    bool Exit();

public:
    int64_t GetMaxSeq();
    std::string GetLastProof();
    std::string GetPreviousHash();
    void Store(std::shared_ptr<xbft::ConsData> p_consensus, const std::string &cr_proof);


private:
    void onTimeout();
    bool startConsensus();

private:
    int64_t m_seq;
    std::string m_lastProof;
    std::string m_previousHash;

    std::shared_ptr<xbft::BftEngine> mp_consensusEngine;
    std::shared_ptr<NodeInfo> mp_nodeInfo;

    utils::TimerLoop m_timerLoop;
    int64_t m_lastConsensusTime;  // 上次出块时间
};

void ValueCommited(std::shared_ptr<xbft::ConsData> p_consData, const std::string &cr_proof);

void SendMsg(const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value);

}  // namespace dealing

#endif
