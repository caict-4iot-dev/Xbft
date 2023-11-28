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

#include "Common.h"
#include "ConsEngine.h"
#include "EventQueue.h"
#include "Singleton.h"
#include "Sync.h"
#include "Timer.h"
#include <string>
#include <thread>


namespace dealing {
class NodeInfo;
class BlockChain : public common::Singleton<BlockChain> {
    friend class common::Singleton<BlockChain>;

public:
    BlockChain();
    ~BlockChain() = default;
    bool Initialize(common::SendMsgFun sendMsg, common::SendMsgTypeFun sendTypeMsg,
        common::EventQueue<std::string> &r_msgQueue, common::EventQueue<common::SyncMsg> &r_msgSyncQueue);
    bool Exit();

public:
    int64_t GetMaxSeq();
    std::string GetLastProof();
    std::string GetPreviousHash();
    void Store(std::shared_ptr<xbft::ConsData> p_consensus, const std::string &cr_proof);  // for consensus
    void Store(int64_t seq, const std::string &cr_hash, const std::string &cr_proof, int64_t closeTime);  // for sync
    void ViewChange();


private:
    void onTimeout();
    bool startConsensus();
    void dealConsensusData(common::EventQueue<std::string> &r_msgQueue);
    void dealSyncData(common::EventQueue<common::SyncMsg> &r_msgSyncQueue);

private:
    int64_t m_seq;
    std::string m_lastProof;
    std::string m_previousHash;

    std::shared_ptr<xbft::BftEngine> mp_consensusEngine;
    std::shared_ptr<NodeInfo> mp_nodeInfo;
    std::shared_ptr<xbft::KeyToolInterface> mp_keyTool;

    utils::TimerLoop m_timerLoop;
    int64_t m_lastConsensusTime;  // 上次出块时间
    int64_t m_lastSyncTime;       // 上次同步时间
    int64_t m_lastConsSeq;

    std::shared_ptr<std::thread> mp_recvNetData;
    std::shared_ptr<std::thread> mp_recvNetSyncData;

    std::shared_ptr<Sync> mp_sync;
};

void ValueCommited(std::shared_ptr<xbft::ConsData> p_consData, const std::string &cr_proof);

void SendMsg(const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value);

void ViewChange();

}  // namespace dealing

#endif
