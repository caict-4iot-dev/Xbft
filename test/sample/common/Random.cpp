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
 * @date: 2023-11-22 16:31:49
 * @file: Random.cpp
 */

#include "Random.h"
#include "Sha256.h"
#include <openssl/rand.h>
#include <cstring>
#include <random>


#define NUM_OS_RANDOM_BYTES 32

namespace common {
int64_t GetPerformanceCounter() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void MemoryClean(void *ptr, size_t len) {
    std::memset(ptr, 0, len);
    __asm__ __volatile__("" : : "r"(ptr) : "memory");
}

void RandAddSeed() {
    // Seed with CPU performance counter
    int64_t nCounter = GetPerformanceCounter();
    RAND_add(&nCounter, sizeof(nCounter), 1.5);
    MemoryClean((void *)&nCounter, sizeof(nCounter));
}

bool GetRandBytes(unsigned char *buf, int num) {
    return RAND_bytes(buf, num) == 1;
}

bool GetOSRand(unsigned char *buf, int num) {
    std::random_device rd;
    for (int i = 0; i < num; i++) {
        buf[i] = (uint8_t)std::uniform_int_distribution<uint16_t>(0, 255)(rd);
    }
    return true;
}

bool GetStrongRandBytes(std::string &out) {
    unsigned char buf[64] = {0};
    RandAddSeed();
    if (!GetRandBytes(buf, 32))
        return false;

    if (!GetOSRand(buf + 32, 32))
        return false;

    std::string input((char *)buf, 64);
    utils::Sha256::Crypto(input, out);
    return true;
}
}  // namespace common