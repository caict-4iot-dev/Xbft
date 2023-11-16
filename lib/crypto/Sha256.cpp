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
 * @author: guojian@caict.ac.cn
 * @date: 2022-06-07 18:18:27
 * @file: Sha256.cpp
 */

#include "Sha256.h"

namespace crypto {
Sha256::Sha256() {
    SHA256_Init(&m_ctx_);
}

void Sha256::Update(const std::string &cr_input) {
    SHA256_Update(&m_ctx_, cr_input.c_str(), cr_input.size());
}

void Sha256::Update(const void *cp_buffer, size_t len) {
    SHA256_Update(&m_ctx_, cp_buffer, len);
}

std::string Sha256::Final() {
    std::string finalStr;
    finalStr.resize(32);
    SHA256_Final((unsigned char *)finalStr.c_str(), &m_ctx_);
    return finalStr;
}

std::string Sha256::Crypto(const std::string &cr_input) {
    std::string strOut = "";
    strOut.resize(32);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, cr_input.c_str(), cr_input.size());
    SHA256_Final((unsigned char *)strOut.c_str(), &sha256);

    return strOut;
}

void Sha256::Crypto(const std::string &cr_input, std::string &r_str) {
    r_str.resize(32);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, cr_input.c_str(), cr_input.size());
    SHA256_Final((unsigned char *)r_str.c_str(), &sha256);
}
}  // namespace crypto
