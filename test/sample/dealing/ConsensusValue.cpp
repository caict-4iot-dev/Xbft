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


namespace dealing {
ConsensusValue::ConsensusValue() {
    m_closeTime = 0;
    m_seq = 0;
    m_previousProof = "";
    m_previousHash = "";
    m_hash = "";
    m_stringValue = "";
    m_value = "";
}

ConsensusValue::ConsensusValue(const std::string &cr_strValue) {
    m_strValue = cr_strValue;
    m_pbValue.ParseFromString(cr_strValue);
    m_hash = crypto::Sha256::Crypto(m_strValue);

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

bool ConsensusValueCheck(std::shared_ptr<ConsData> p_consData) {
    LOG_INFO("ConsensusValueCheck .......");
    std::shared_ptr<ConsensusValue> consensusValue = std::static_pointer_cast<ConsensusValue>(p_consData);
    if (consensusValue == nullptr) {
        LOG_ERROR("ConsensusValueCheck Error");
        return false;
    }
    auto closeTime = consensusValue->GetCloseTime();
    if (closeTime > utils::TimeStamp::HighResolution()) {
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

std::shared_ptr<ConsData> ParseStringToConsData(const std::string &cr_input) {
    LOG_INFO("ParseStringToConsData .......");
    auto cons = std::make_shared<ConsensusValue>(cr_input);
    return cons;
}

}  // namespace dealing