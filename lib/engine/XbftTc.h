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
 * @file: XbftTc.h
 */

#ifndef __XBFTTC_H__
#define __XBFTTC_H__

#include "XbftValidatorSet.h"
#include "consensus.pb.h"
#include <memory>

namespace xbft {
struct KeyToolInterface;
class XbftTc {
public:
    XbftTc(const protocol::XbftTc &timeoutCert, std::shared_ptr<KeyToolInterface> p_keyTool);
    ~XbftTc();

    protocol::XbftQc GetHighQc();

    bool IsValid(const XbftValidatorSet &validators, size_t quorumSize) const;

private:
    protocol::XbftTc m_tc;
    std::shared_ptr<KeyToolInterface> mp_keyTool;
};
}  // namespace xbft

#endif