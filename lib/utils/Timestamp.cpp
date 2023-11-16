/*!
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
 * @author: maruolong@caict.ac.cn
 * @date: 2022-06-16 15:30:24
 * @file: Timestamp.cpp
 */

#include "Timestamp.h"
#include <ctime>

namespace utils {

Timestamp::Timestamp() : m_begin(std::chrono::steady_clock::now()), m_now(std::chrono::system_clock::now()) {}

void Timestamp::Reset() {
    m_begin = std::chrono::steady_clock::now();
}

bool Timestamp::TimeOut(int nTime) {
    bool timeout = false;
    auto endTime = std::chrono::steady_clock::now();
    timeout = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_begin) >
              std::chrono::microseconds(nTime * 1000 * 1000);

    return timeout;
}

int64_t Timestamp::Elapsed(Ratio ratio) const {
    int64_t timestamp = 0;
    switch (ratio) {
    case Hours:
        timestamp = std::chrono::duration_cast<std::chrono::hours>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    case Minutes:
        timestamp =
            std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    case Seconds:
        timestamp =
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    case Milliseconds:
        timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    case Microseconds:
        timestamp =
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    case Nanoseconds:
        timestamp =
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    default:
        timestamp =
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_begin).count();
        break;
    }
    return timestamp;
}

int64_t Timestamp::HighResolution() {
    auto timeNow = std::chrono::system_clock::now();
    auto mircoDuration = std::chrono::duration_cast<std::chrono::microseconds>(timeNow.time_since_epoch());
    return mircoDuration.count();
}

std::string Timestamp::FormatDateString() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::ctime(&now);
}

Timestamp Timestamp::Now() {
    return Timestamp();
}

}  // namespace utils
