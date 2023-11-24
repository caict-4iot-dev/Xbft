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
 * @date: 2023-11-22 16:29:30
 * @file: Ed25519.h
 */

#ifndef _ED25519_H_
#define _ED25519_H_

#include "ConsEngine.h"
#include <string>

namespace common {
class Ed25519 : public xbft::KeyInterface {
public:
    Ed25519();             // construct
    ~Ed25519() = default;  // destruct
    Ed25519(const std::string &cr_priKey);

    std::string Sign(const std::string &cr_input);
    std::string GetPublicKey();
    std::string GetPrivateKey();
    std::string GetAddress();

private:
    void from(const std::string &cr_sPrivateKey);

    std::string m_privateKey;
    std::string m_publickKey;
};

bool Verify(const std::string &cr_publicKey, const std::string &cr_inputMsg, const std::string &cr_sig);
std::string PublicKeyToAddress(const std::string &cr_publicKey);

}  // namespace common

#endif  //_ED25519_H_