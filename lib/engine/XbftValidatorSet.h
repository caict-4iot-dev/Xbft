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
 * @file: XbftValidatorSet.h
 */

#ifndef __XBFTVALIDATORSET_H__
#define __XBFTVALIDATORSET_H__

#include "consensus.pb.h"
#include <map>
#include <string>

namespace xbft {

using ValidatorMap = std::map<std::string, int64_t>;
class XbftValidatorSet {
public:
    XbftValidatorSet();
    ~XbftValidatorSet();

    bool Insert(const std::string &cr_addr, int64_t id);
    void Update(const std::string &cr_addr, int64_t id);
    std::string QueryAddr(int64_t id) const;
    int64_t QueryId(const std::string &cr_addr) const;
    size_t Size() const;
    bool Empty() const;
    size_t GetQuorumSize() const;
    bool Compare(const protocol::ValidatorSet &cr_validators) const;
    void Set(const protocol::ValidatorSet &cr_validators);
    int64_t GetValidatorId(const std::string &cr_addr) const;
    void GetOtherReplicaAddrs(std::vector<std::string> &r_replicaAddrs, int64_t replicaId) const;

private:
    ValidatorMap m_vs;
};

}  // namespace xbft
#endif
