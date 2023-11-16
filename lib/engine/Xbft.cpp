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
 * @date: 2023-11-15 17:15:33
 * @file: Xbft.cpp
 */

#include "ConsEngine.h"
#include "XbftEngine.h"


namespace xbft {
std::shared_ptr<BftEngine> CreateXbftEngine(std::shared_ptr<NetInterface> p_net,
    std::shared_ptr<ValueDealInterface> p_valueDeal, std::shared_ptr<NodeInfoInterface> p_nodeInfo,
    std::shared_ptr<KeyToolInterface> p_keyTool) {
    auto engine = std::make_shared<XbftEngine>(p_net, p_valueDeal, p_keyTool);
    if (!engine->StartEngine(p_nodeInfo)) {
        return nullptr;
    }
    return engine;
}

}  // namespace xbft