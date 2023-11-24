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
 * @date: 2023-11-23 17:07:02
 * @file: NodeInfo.cpp
 */

#include "NodeInfo.h"
#include "Configure.h"
#include "Ed25519.h"
#include "Logger.h"
#include "utils/Strings.h"


namespace dealing {
NodeInfo::NodeInfo() {
    mp_key = nullptr;
    m_interval = 1000;
    m_blockInterVal = 1000;
    m_lastProof = "";
    m_viewNumber = 0;
    m_validators.clear();
}

bool NodeInfo::LoadConfig() {
    std::string pri = utils::String::HexStringToBin(common::NodeConfig::m_priKey);
    mp_key = std::make_shared<common::Ed25519>(pri);
    if (mp_key == nullptr) {
        LOG_ERROR("NodeInfo::LoadConfig Ed25519 nullptr");
        return false;
    }
    m_interval = common::ConsConfig::m_interval;
    m_blockInterVal = common::NodeConfig::m_interval;

    m_validators = common::ConsConfig::m_validators;
    if (m_validators.size() < 4) {
        LOG_ERROR("NodeInfo::LoadConfig validators < 4, size:%d", m_validators.size());
        return false;
    }
    return true;
}
}  // namespace dealing