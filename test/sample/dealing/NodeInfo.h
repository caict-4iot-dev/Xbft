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
 * @date: 2023-11-23 17:03:35
 * @file: NodeInfo.h
 */

#ifndef __NODEINFO_H__
#define __NODEINFO_H__

#include "ConsEngine.h"
#include <memory>
#include <string>
#include <vector>


namespace dealing {

class NodeInfo : public xbft::NodeInfoInterface {
public:
    NodeInfo();
    ~NodeInfo() = default;

    bool LoadConfig();

    std::vector<std::string> GetValidators() { return m_validators; };
    std::string GetLastProof() { return m_lastProof; };
    int64_t GetLastViewNumber() { return m_viewNumber; };
    std::shared_ptr<xbft::KeyInterface> GetKeyInfo() { return mp_key; };
    int64_t GetSendMsgInterval() { return m_interval; };

public:
    int64_t GetBlockInterVal() { return m_blockInterVal; };

private:
    std::shared_ptr<xbft::KeyInterface> mp_key;
    int64_t m_interval;
    int64_t m_blockInterVal;
    std::string m_lastProof;
    int64_t m_viewNumber;
    std::vector<std::string> m_validators;
};
}  // namespace dealing


#endif
