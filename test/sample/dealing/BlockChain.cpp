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
 * @date: 2023-11-22 18:11:34
 * @file: BlockChain.cpp
 */


#include "BlockChain.h"
#include "ConsensusValue.h"
#include "Ed25519.h"
#include "ExitHandler.h"
#include "Logger.h"
#include "NodeInfo.h"
#include "Timestamp.h"
#include "Xbft.h"
#include "utils/Strings.h"

namespace dealing {
BlockChain::BlockChain() {
    m_seq = 1;
    m_lastProof = "";
    m_previousHash = "";
    mp_consensusEngine = nullptr;
    m_lastConsensusTime = 0;
    mp_recvNetData = nullptr;
    mp_keyTool = nullptr;
}

bool BlockChain::Initialize(std::shared_ptr<xbft::NetInterface> p_net, common::EventQueue<std::string> &r_msgQueue) {
    // 构建nodeInfo
    mp_nodeInfo = std::make_shared<NodeInfo>();
    if (mp_nodeInfo == nullptr) {
        LOG_ERROR("BlockChain::Initialize nodeInfo nullptr");
        return false;
    }

    // 加载配置数据
    if (!mp_nodeInfo->LoadConfig()) {
        LOG_ERROR("BlockChain::Initialize LoadConfig Error");
        return false;
    }

    // 构建valueDeal
    auto valueDeal = std::make_shared<xbft::ValueDealInterface>();
    if (valueDeal == nullptr) {
        LOG_ERROR("BlockChain::Initialize valueDeal nullptr");
        return false;
    }
    valueDeal->m_checkValue = dealing::ConsensusValueCheck;
    valueDeal->m_valueCommited = dealing::ValueCommited;
    valueDeal->m_viewChange = dealing::ViewChange;

    // 构建keyTool
    mp_keyTool = std::make_shared<xbft::KeyToolInterface>();
    if (mp_keyTool == nullptr) {
        LOG_ERROR("BlockChain::Initialize keyTool nullptr");
        return false;
    }
    mp_keyTool->m_verify = common::Verify;
    mp_keyTool->m_publicKeyToAddr = common::PublicKeyToAddress;
    mp_keyTool->m_createConsData = dealing::ParseStringToConsData;

    mp_consensusEngine = xbft::CreateXbftEngine(p_net, valueDeal, mp_nodeInfo, mp_keyTool);
    if (mp_consensusEngine == nullptr) {
        LOG_ERROR("BlockChain::Initialize CreateXbftEngine nullptr");
        return false;
    }

    mp_recvNetData = std::make_shared<std::thread>(&BlockChain::dealConsensusData, this, std::ref(r_msgQueue));
    if (mp_recvNetData == nullptr) {
        LOG_ERROR("BlockChain::Initialize thread nullptr");
        return false;
    }

    // 延迟3s启动
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    // 启动定时器 打包判断 及 view-change判断
    m_timerLoop.SetTimeOut(1000, [this]() { onTimeout(); });

    return true;
}

void BlockChain::dealConsensusData(common::EventQueue<std::string> &r_msgQueue) {
    while (!common::ExitHandler::GetExitFlag()) {
        std::string msg = "";
        if (!r_msgQueue.TryPop(std::chrono::milliseconds(100), msg)) {
            continue;
        }

        xbft::Recv(mp_consensusEngine, msg, mp_keyTool);
    }
}

bool BlockChain::Exit() {
    LOG_INFO("BlockChain Exit");
    mp_recvNetData->join();
    m_timerLoop.Stop();
    LOG_INFO("BlockChain Exit OK");
    return true;
}

int64_t BlockChain::GetMaxSeq() {
    return m_seq;
}

std::string BlockChain::GetLastProof() {
    return m_lastProof;
}

std::string BlockChain::GetPreviousHash() {
    return m_previousHash;
}

void BlockChain::Store(std::shared_ptr<xbft::ConsData> p_consensus, const std::string &cr_proof) {
    std::shared_ptr<ConsensusValue> consensusValue = std::static_pointer_cast<ConsensusValue>(p_consensus);
    m_seq = consensusValue->GetSeq();
    m_previousHash = consensusValue->GetHash();
    m_lastProof = cr_proof;

    m_lastConsensusTime = consensusValue->GetCloseTime();
    LOG_INFO("Store seq:%ld, previousHash:%s", m_seq, utils::String::BinToHexString(m_previousHash).c_str());
}

void BlockChain::ViewChange() {
    LOG_INFO("BlockChain viewChange over");
    m_lastConsensusTime = utils::Timestamp::HighResolution();
}

bool BlockChain::startConsensus() {
    // is viewActive
    if (!xbft::IsViewActive(mp_consensusEngine)) {
        return true;
    }

    // is leader?
    if (!xbft::IsLeader(mp_consensusEngine)) {
        LOG_INFO("StartConsensus Current Node Not Leader, seq:%ld, view-number:%ld", m_seq,
            xbft::GetViewNumber(mp_consensusEngine));
        return true;
    }

    // 构建consensusValue数据
    auto consValue = std::make_shared<ConsensusValue>();
    if (consValue == nullptr) {
        LOG_ERROR("StartConsensus consensusValue nullptr");
        return false;
    }

    m_lastConsensusTime = utils::Timestamp::HighResolution();
    LOG_INFO("BlockChain::startConsensus current:%ld", m_seq);

    consValue->SetCloseTime(m_lastConsensusTime);
    consValue->SetSeq(m_seq + 1);
    consValue->SeqPreviousProof(m_lastProof);
    consValue->SetPreviousHash(m_previousHash);
    consValue->SetValue("test value");

    consValue->SetProtobufData();

    // 发起提案
    return xbft::Propose(mp_consensusEngine, consValue);
}

void BlockChain::onTimeout() {
    // 检测是否发起打包 BlockInterVal间隔
    if (utils::Timestamp::HighResolution() - m_lastConsensusTime > mp_nodeInfo->GetBlockInterVal()) {
        startConsensus();
    }
    // 检测是否超过配置的出块时间 5 * BlockInterVal
    if (utils::Timestamp::HighResolution() - m_lastConsensusTime > 5 * mp_nodeInfo->GetBlockInterVal()) {
        // 发起view-change
        if (xbft::IsViewActive(mp_consensusEngine)) {
            LOG_INFO("BlockChain view-change trigger");
            xbft::Rotate(mp_consensusEngine);
        }
    }
}

void ValueCommited(std::shared_ptr<xbft::ConsData> p_consData, const std::string &cr_proof) {
    BlockChain::Instance().Store(p_consData, cr_proof);
}

void ViewChange() {
    BlockChain::Instance().ViewChange();
}

}  // namespace dealing