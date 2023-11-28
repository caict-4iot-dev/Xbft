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
 * @date: 2023-11-27 18:07:12
 * @file: Sync.h
 */

#ifndef __SYNC_H__
#define __SYNC_H__

#include "Common.h"
#include <string>
#include <vector>

namespace dealing {
class Sync {
public:
    Sync(common::SendMsgTypeFun sendType, const std::string &cr_address, const std::vector<std::string> &cr_validators);
    ~Sync() = default;
    void SendSyncRequest();
    void SendSyncResponse(const std::string &address, int64_t seq, const std::string &cr_hash, const std::string &cr_proof, int64_t closeTime);
    void RecvSyncResponse(const std::string &cr_value, int64_t &seq, std::string &r_hash, std::string &r_proof, int64_t &r_closeTime);

private:
    common::SendMsgTypeFun m_send;
    std::string m_address;
    std::vector<std::string> m_otherAddress;
};
}  // namespace dealing

#endif
