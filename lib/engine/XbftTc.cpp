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
 * @file: XbftTc.cpp
 */


#include "XbftTc.h"
#include "ConsEngine.h"
#include "Logger.h"

namespace xbft {
XbftTc::XbftTc(const protocol::XbftTc &cr_timeoutCert, std::shared_ptr<KeyToolInterface> p_keyTool) {
    m_tc = cr_timeoutCert;
    mp_keyTool = p_keyTool;
}

XbftTc::~XbftTc() = default;

bool XbftTc::IsValid(const XbftValidatorSet &cr_validators, size_t quorumSize) const {
    // 检查high_qc_view_number队列和signature队列长度一致
    if (m_tc.high_qc_view_number_size() != m_tc.signature_size()) {
        return false;
    }

    // 检查对签名都是有效的
    std::set<std::string> validValidators;
    for (int i = 0; i < m_tc.high_qc_view_number_size(); i++) {
        // signature
        const protocol::Signature &sig = m_tc.signature(i);
        std::string addr = mp_keyTool->m_publicKeyToAddr(sig.public_key());
        if (cr_validators.QueryId(addr) == -1) {
            LOG_WARN("Cann't find the tc'validator from list");
            continue;
        }

        // data
        char strSignedViewNumber[2 * sizeof(int64_t)];
        *(int64_t *)strSignedViewNumber = m_tc.view_number();
        *(int64_t *)(strSignedViewNumber + sizeof(int64_t)) = m_tc.high_qc_view_number(i);
        if (!mp_keyTool->m_verify(strSignedViewNumber, sig.sign_data(), sig.public_key())) {
            LOG_WARN("Failed to verify the tc's signature");
            continue;
        }
        validValidators.insert(addr);
    }

    return validValidators.size() >= quorumSize;
}
}  // namespace xbft
