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
 * @date: 2023-11-15 15:08:21
 * @file: XbftValidatorSet.cpp
 */

#include "XbftValidatorSet.h"

namespace xbft {
XbftValidatorSet::XbftValidatorSet() {}

XbftValidatorSet::~XbftValidatorSet() = default;

bool XbftValidatorSet::Insert(const std::string &cr_addr, int64_t id) {
    return m_vs.insert(std::make_pair(cr_addr, id)).second;
}

void XbftValidatorSet::Update(const std::string &cr_addr, int64_t id) {
    m_vs[cr_addr] = id;
}

std::string XbftValidatorSet::QueryAddr(int64_t id) const {
    for (auto item : m_vs) {
        if (item.second == id) {
            return item.first;
        }
    }
    return "";
}

int64_t XbftValidatorSet::QueryId(const std::string &cr_addr) const {
    auto iter = m_vs.find(cr_addr);
    if (iter != m_vs.end()) {
        return iter->second;
    }
    return -1;
}

size_t XbftValidatorSet::Size() const {
    return m_vs.size();
}

bool XbftValidatorSet::Empty() const {
    return m_vs.empty();
}

size_t XbftValidatorSet::GetQuorumSize() const {
    return m_vs.size() - m_vs.size() / 3;
}

void XbftValidatorSet::Set(const protocol::ValidatorSet &cr_validators) {
    m_vs.clear();
    int64_t counter = 0;
    for (int32_t i = 0; i < cr_validators.validators_size(); i++) {
        m_vs.insert(std::make_pair(cr_validators.validators(i).address(), counter++));
    }
}

bool XbftValidatorSet::Compare(const protocol::ValidatorSet &cr_validators) const {
    bool validatorChanged = (cr_validators.validators_size() != (int32_t)m_vs.size());
    int32_t validatorIndex = 0;
    for (int32_t i = 0; i < cr_validators.validators_size(); i++) {
        auto iter = m_vs.find(cr_validators.validators(i).address());
        if (iter == m_vs.end()) {
            validatorChanged = true;
            break;
        }

        if (iter->second != validatorIndex++) {
            validatorChanged = true;
            break;
        }
    }

    return validatorChanged;
}

int64_t XbftValidatorSet::GetValidatorId(const std::string &cr_addr) const {
    auto iter = m_vs.find(cr_addr);
    if (iter != m_vs.end()) {
        return iter->second;
    }
    return -1;
}

void XbftValidatorSet::GetOtherReplicaAddrs(std::vector<std::string> &r_replicaAddrs, int64_t replicaId) const {
    for (auto item : m_vs) {
        if (item.second != replicaId) {
            r_replicaAddrs.push_back(item.first);
        }
    }
}
}  // namespace xbft
