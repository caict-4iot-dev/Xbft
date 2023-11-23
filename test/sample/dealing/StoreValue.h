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
 * @file: StoreValue.h
 */

#ifndef __STORE_VALUE_H__
#define __STORE_VALUE_H__

namespace dealing {

#include "ConsEngine.h"
#include <string>

class StoreValue : public common::Singleton<StoreValue> {
    friend class utils::Singleton<StoreValue>;

public:
    StoreValue();
    int64_t GetMaxSeq();
    std::string GetLastProof();
    std::string GetPreviousHash();
    void Store(std::shared_ptr<xbft::ConsData> p_consensus, const std::string &cr_proof);

    void Dealing(size_t index);

private:
    int64_t m_seq;
    std::string m_lastProof;
    std::string m_previousHash;
};

static void ValueCommited(std::shared_ptr<xbft::ConsData> p_consData, const std::string &cr_proof);

}  // namespace dealing

#endif
