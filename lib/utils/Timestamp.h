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
 * @date: 2022-06-16 15:30:19
 * @file: Timestamp.h
 */

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <chrono>
#include <string>

namespace utils {

enum Ratio {
    Hours = 0,
    Minutes,
    Seconds,
    Milliseconds,
    Microseconds,
    Nanoseconds
};

class Timestamp {
public:
    Timestamp();
    // Timestamp(int64_t usTimestamp);
    // Timestamp(const Timestamp &ts);

    ~Timestamp() = default;                             // destruct
    Timestamp(const Timestamp &a) = delete;             // copy construct
    Timestamp(Timestamp &&) = delete;                   // move construct
    Timestamp &operator=(const Timestamp &a) = delete;  // assign
    Timestamp &operator=(Timestamp &&) = delete;        // move

    static int64_t HighResolution();
    static std::string FormatDateString();
    static Timestamp Now();

    void Reset();
    bool TimeOut(int nTime);

    // default microseconds
    int64_t Elapsed(Ratio ratio = Microseconds) const;

private:
    std::chrono::time_point<std::chrono::steady_clock> m_begin;
    std::chrono::time_point<std::chrono::system_clock> m_now;
};

}  // namespace utils
#endif  // Timestamp_h__
