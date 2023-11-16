/*!
*© COPYRIGHT 2022 Corporation CAICT All rights reserved.
 * http://www.caict.ac.cn
 * https://bitfactory.cn

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 * @brief:
 * @file:   Timer.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/06/18  14:02:29
 */

#ifndef __TIMER_H__
#define __TIMER_H__
#include "FunctionWraper.h"
#include <atomic>
#include <functional>
#include <memory>
#include <thread>
namespace utils {

class TimerBase {
public:
    TimerBase() {
        m_isRun = true;
        m_threadVec.clear();
    };                                                  // construct
    virtual ~TimerBase() { Stop(); };                   // destruct
    TimerBase(const TimerBase &a) = delete;             // copy construct
    TimerBase(TimerBase &&) = delete;                   // move construct
    TimerBase &operator=(const TimerBase &a) = delete;  // assign
    TimerBase &operator=(TimerBase &&) = delete;        // move
    virtual void Stop() {
        if (m_threadVec.size() != 0 && m_isRun) {
            m_isRun = false;
            for (auto &i : m_threadVec) {
                if (i->joinable()) {
                    i->join();
                }
            }
            m_threadVec.clear();
        }
    };

protected:
    std::atomic_bool m_isRun;
    std::vector<std::shared_ptr<std::thread>> m_threadVec;
};

class TimerFor final : public TimerBase {
public:
    TimerFor() = default;                             // construct
    ~TimerFor(){};                                    // destruct
    TimerFor(const TimerFor &a) = delete;             // copy construct
    TimerFor(TimerFor &&) = delete;                   // move construct
    TimerFor &operator=(const TimerFor &a) = delete;  // assign
    TimerFor &operator=(TimerFor &&) = delete;        // move

    template <typename Function, typename... Args>
    void SetTimeOut(int delay /*delay ms*/, Function &&rr_func, Args &&...args) {
        m_isRun = true;
        std::shared_ptr<std::thread> p_thread =
            std::make_shared<std::thread>([this, delay, rr_func = std::forward<Function>(rr_func),
                                              args = std::make_tuple(std::forward<Args>(args)...)]() {
                if (!m_isRun)
                    return;
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                if (!m_isRun)
                    return;
                std::apply(rr_func, args);
            });
        m_threadVec.push_back(p_thread);
    }
};
class TimerUntil final : public TimerBase {
public:
    TimerUntil() = default;                               // construct
    ~TimerUntil(){};                                      // destruct
    TimerUntil(const TimerUntil &a) = delete;             // copy construct
    TimerUntil(TimerUntil &&) = delete;                   // move construct
    TimerUntil &operator=(const TimerUntil &a) = delete;  // assign
    TimerUntil &operator=(TimerUntil &&) = delete;        // move

    template <typename _Clock, typename _Duration, typename Function, typename... Args>
    void SetTimeOut(const std::chrono::time_point<_Clock, _Duration> &cr_point, Function &&rr_func, Args &&...args) {
        m_isRun = true;
        std::shared_ptr<std::thread> p_thread =
            std::make_shared<std::thread>([this, cr_point, rr_func = std::forward<Function>(rr_func),
                                              args = std::make_tuple(std::forward<Args>(args)...)]() {
                if (!m_isRun)
                    return;
                std::this_thread::sleep_until(cr_point);
                if (!m_isRun)
                    return;
                std::apply(rr_func, args);
            });
        m_threadVec.push_back(p_thread);
    }
};

class TimerLoop final : public TimerBase {
public:
    TimerLoop() = default;                              // construct
    ~TimerLoop(){};                                     // destruct
    TimerLoop(const TimerLoop &a) = delete;             // copy construct
    TimerLoop(TimerLoop &&) = delete;                   // move construct
    TimerLoop &operator=(const TimerLoop &a) = delete;  // assign
    TimerLoop &operator=(TimerLoop &&) = delete;        // move

    template <typename Function, typename... Args>
    void SetTimeOut(int delay, Function &&rr_func, Args &&...args) {
        m_isRun = true;
        std::shared_ptr<std::thread> p_thread =
            std::make_shared<std::thread>([this, delay, rr_func = std::forward<Function>(rr_func),
                                              args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                while (true) {
                    if (!m_isRun)
                        return;
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                    if (!m_isRun)
                        return;
                    std::apply(rr_func, args);
                }
            });
        m_threadVec.push_back(p_thread);
    }
};
}  // namespace utils

#endif  // Timer_h__