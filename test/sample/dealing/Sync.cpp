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
 * @date: 2023-11-27 18:12:09
 * @file: Sync.cpp
 */

#include "Sync.h"
#include "chain.pb.h"

namespace dealing {

Sync::Sync(
    common::SendMsgTypeFun sendType, const std::string &cr_address, const std::vector<std::string> &cr_validators) {
    m_send = sendType;
    m_address = cr_address;
    m_otherAddress.clear();
    for (auto validator : cr_validators) {
        if (validator != m_address) {
            m_otherAddress.push_back(validator);
        }
    }
}

void Sync::SendSyncRequest() {
    m_send(m_address, m_otherAddress, "", true);
}

void Sync::SendSyncResponse(const std::string &cr_address, int64_t seq, const std::string &cr_hash,
    const std::string &cr_proof, int64_t closeTime) {
    protocol::SyncValue sync;
    sync.set_ledger_seq(seq);
    sync.set_previous_hash(cr_hash);
    sync.set_previous_proof(cr_proof);
    sync.set_close_time(closeTime);

    std::vector<std::string> dest;
    dest.push_back(cr_address);

    m_send(m_address, dest, sync.SerializeAsString(), false);
}

void Sync::RecvSyncResponse(
    const std::string &cr_value, int64_t &seq, std::string &r_hash, std::string &r_proof, int64_t &r_closeTime) {
    protocol::SyncValue sync;
    sync.ParseFromString(cr_value);
    seq = sync.ledger_seq();
    r_hash = sync.previous_hash();
    r_proof = sync.previous_proof();
    r_closeTime = sync.close_time();
}

}  // namespace dealing
