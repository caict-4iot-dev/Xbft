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
 * @date: 2023-11-22 17:11:22
 * @file: ConsensusValue.cpp
 */

#include "ConsensusValue.h"
#include "Logger.h"
#include "Sha256.h"
#include "Timestamp.h"
#include "utils/Strings.h"

namespace dealing {
ConsensusValue::ConsensusValue() {
    m_closeTime = 0;
    m_seq = 0;
    m_previousProof = "";
    m_previousHash = "";
    m_hash = "";
    m_value = "";
    m_strValue = "";
}

ConsensusValue::ConsensusValue(const std::string &cr_strValue) {
    m_strValue = cr_strValue;
    m_pbValue.ParseFromString(cr_strValue);
    m_hash = utils::Sha256::Crypto(m_strValue);

    m_closeTime = m_pbValue.close_time();
    m_seq = m_pbValue.ledger_seq();
    m_previousProof = m_pbValue.previous_proof();
    m_previousHash = m_pbValue.previous_hash();
    m_value = m_pbValue.value();
}

int64_t ConsensusValue::GetCloseTime() {
    return m_closeTime;
}

int64_t ConsensusValue::GetSeq() {
    return m_seq;
}

std::string ConsensusValue::GetPreviousProof() {
    return m_previousProof;
}

std::string ConsensusValue::GetPreviousHash() {
    return m_previousHash;
}

std::string ConsensusValue::GetValue() {
    return m_value;
}

std::string ConsensusValue::GetHash() {
    return m_hash;
}

std::string ConsensusValue::GetStringValue() {
    return m_strValue;
}

void ConsensusValue::SetCloseTime(int64_t time) {
    m_closeTime = time;
}

void ConsensusValue::SetSeq(int64_t seq) {
    m_seq = seq;
}

void ConsensusValue::SeqPreviousProof(const std::string &cr_previousProof) {
    m_previousProof = cr_previousProof;
}

void ConsensusValue::SetPreviousHash(const std::string &cr_previousHash) {
    m_previousHash = cr_previousHash;
}

void ConsensusValue::SetValue(const std::string &cr_value) {
    m_value = cr_value;
}

void ConsensusValue::SetProtobufData() {
    m_pbValue.set_close_time(m_closeTime);
    m_pbValue.set_ledger_seq(m_seq);
    m_pbValue.set_previous_hash(m_previousHash);
    m_pbValue.set_previous_proof(m_previousProof);
    m_pbValue.set_value(m_value);

    m_strValue = m_pbValue.SerializeAsString();

    m_hash = utils::Sha256::Crypto(m_strValue);
}

/*
    针对待共识数据结构进行验证；如共识结构高度、共识结构生成时间、待共识数据大小等内容；
*/
bool ConsensusValueCheck(std::shared_ptr<xbft::ConsData> p_consData) {
    std::shared_ptr<ConsensusValue> consensusValue = std::static_pointer_cast<ConsensusValue>(p_consData);
    if (consensusValue == nullptr) {
        LOG_ERROR("ConsensusValueCheck Error");
        return false;
    }
    auto closeTime = consensusValue->GetCloseTime();
    if (closeTime > utils::Timestamp::HighResolution()) {
        LOG_ERROR("close time invalid");
        return false;
    }

    auto seq = consensusValue->GetSeq();
    if (seq < 0) {
        LOG_ERROR("seq invalid");
        return false;
    }
    LOG_INFO("ConsensusValueCheck closeTime:%ld seq:%ld", closeTime, seq);
    return true;
}

/*
    共识数据构建，通过string构建consData结构
*/
std::shared_ptr<xbft::ConsData> ParseStringToConsData(const std::string &cr_input) {
    auto cons = std::make_shared<ConsensusValue>(cr_input);
    return cons;
}

}  // namespace dealing
