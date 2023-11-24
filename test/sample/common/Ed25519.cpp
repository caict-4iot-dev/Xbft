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
 * @file: Ed25519.cpp
 */


#include "Ed25519.h"
#include "Random.h"
#include <ed25519/ed25519.h>

namespace common {
Ed25519::Ed25519() {
    GetStrongRandBytes(m_privateKey);
    m_publickKey.resize(32);
    ed25519_publickey((const unsigned char *)m_privateKey.c_str(), (unsigned char *)m_publickKey.c_str());
}
Ed25519::Ed25519(const std::string &cr_priKey) {
    from(cr_priKey);
}

std::string Ed25519::Sign(const std::string &cr_input) {
    unsigned char sig[10240];
    unsigned int sigLen = 64;
    ed25519_sign((unsigned char *)cr_input.c_str(), cr_input.size(), (const unsigned char *)m_privateKey.c_str(),
        (unsigned char *)m_publickKey.c_str(), sig);
    std::string output;
    output.append((const char *)sig, sigLen);
    return output;
}

std::string Ed25519::GetPublicKey() {
    return m_publickKey;
}

std::string Ed25519::GetPrivateKey() {
    return m_privateKey;
}

std::string Ed25519::GetAddress() {
    return PublicKeyToAddress(m_publickKey);
}

void Ed25519::from(const std::string &cr_sPrivateKey) {
    m_privateKey = cr_sPrivateKey;
    m_publickKey.resize(32);
    ed25519_publickey((const unsigned char *)m_privateKey.c_str(), (unsigned char *)m_publickKey.c_str());
}

bool Verify(const std::string &cr_publicKey, const std::string &cr_inputMsg, const std::string &cr_sig) {
    int res = ed25519_sign_open((unsigned char *)cr_inputMsg.c_str(), cr_inputMsg.size(),
        (unsigned char *)cr_publicKey.c_str(), (unsigned char *)cr_sig.c_str());
    return 0 == res;
}

std::string PublicKeyToAddress(const std::string &cr_publicKey) {
    if (cr_publicKey.size() > 20)
        return cr_publicKey.substr(0, 20);
    return "";
}

}  // namespace common
