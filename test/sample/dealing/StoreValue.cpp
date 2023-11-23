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
 * @file: StoreValue.cpp
 */

#include "StoreValue.h"
#include "ConsensusValue.h"
#include "Logger.h"
#include "Strings.h"

namespace dealing {
StoreValue::StoreValue() {
    m_seq = 0;
    m_lastProof = "";
}

int64_t StoreValue::GetMaxSeq() {
    return m_seq;
}

std::string StoreValue::GetLastProof() {
    return m_lastProof;
}

std::string StoreValue::GetLastProof() {
    return m_previousHash;
}

void StoreValue::Store(std::shared_ptr<xbft::ConsData> p_consensus, const std::string &cr_proof) {
    std::shared_ptr<ConsensusValue> consensusValue = std::static_pointer_cast<ConsensusValue>(p_consData);
    m_seq = consensusValue->GetSeq();
    m_previousHash = consensusValue->GetHash();
    m_lastProof = cr_proof;
    LOG_INFO("Store seq:%ld, previousHash:%s", m_seq, utils::String::BinToHexString(m_previousHash).c_str());
}

void StoreValue::Dealing() {
    // 构建共识节点
}

void ValueCommited(std::shared_ptr<xbft::ConsData> p_consData, const std::string &cr_proof) {
    StoreValue::Store(p_consData, cr_proof);
}

}  // namespace dealing