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
 * @file: Sha256.h
 */

#ifndef __SHA256_H__
#define __SHA256_H__

#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <string>

namespace crypto {
class Hash {
public:
    Hash() = default;
    ~Hash() = default;

    virtual void Update(const std::string &cr_input) = 0;
    virtual void Update(const void *cp_buffer, size_t len) = 0;
    virtual std::string Final() = 0;
};

class Sha256 : public Hash {
public:
    Sha256();
    ~Sha256() = default;

    void Update(const std::string &cr_input) override;
    void Update(const void *cp_buffer, size_t len) override;
    std::string Final() override;

    static std::string Crypto(const std::string &cr_input);
    static void Crypto(const std::string &cr_input, std::string &r_str);

public:
    static const int32_t SIZE = 32;

private:
    SHA256_CTX m_ctx_;
};
}  // namespace crypto

#endif  //_SHA256_H_