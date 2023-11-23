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
 * @date: 2023-11-22 16:59:49
 * @file: ConsensusValue.h
 */

#ifndef __CONSENSUS_VALUE_H__
#define __CONSENSUS_VALUE_H__

#include "ConsEngine.h"
#include "chain.pb.h"

namespace dealing {
class ConsensusValue : public xbft::ConsData {
public:
    using ConsensusValuePtr = std::shared_ptr<ConsensusValue>;

public:
    ConsensusValue();                                // construct
    ConsensusValue(const std::string &cr_strValue);  // construct
    ~ConsensusValue() = default;                     // destruct

    int64_t GetCloseTime();
    int64_t GetSeq();
    std::string GetPreviousProof();
    std::string GetPreviousHash();
    std::string GetValue();

    std::string GetHash();
    std::string GetStringValue();

private:
    int64_t m_closeTime;
    int64_t m_seq;
    std::string m_previousProof;
    std::string m_previousHash;
    std::string m_hash;
    std::string m_value;

    std::string m_strValue;
    protocol::ConsensusValue m_pbValue;
};

static bool ConsensusValueCheck(std::shared_ptr<ConsData> p_consData);
static std::shared_ptr<ConsData> ParseStringToConsData(const std::string &cr_input);

}  // namespace dealing


#endif
