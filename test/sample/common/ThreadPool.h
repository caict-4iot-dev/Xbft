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
 * @file:   ThreadPool.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/03/03  14:16:22
 */
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <atomic>
#include <condition_variable>

#include "FunctionWraper.h"
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>
#include <sched.h>
namespace common {

static const std::size_t MAX_SIZE = 100;

class ThreadPool final {
public:
    explicit ThreadPool() : m_isStop(false), m_taskSize(0) {}

    ~ThreadPool() { Stop(); }

    bool InitThreadNum(std::size_t num, bool bindCpu = false);
    std::size_t GetSize();
    template <typename Function, typename... Args>
    decltype(auto) AddTask(Function &&rr_func, Args &&...args) {
        using ReturnType = decltype(std::invoke(rr_func, std::forward<Args>(args)...));

        std::shared_ptr<std::promise<ReturnType>> promise = std::make_shared<std::promise<ReturnType>>();
        std::future<ReturnType> ret_future = promise->get_future();

        FuncWrapper<ReturnType> func_wrapper;
        std::function<void()> wrapped_func =
            func_wrapper.GetWrapped(std::move(rr_func), std::move(promise), std::forward<Args>(args)...);

        {
            std::unique_lock<std::mutex> locker(m_mutex);
            m_taskQueue.emplace(std::move(wrapped_func));
            m_taskSize++;
        }
        m_taskGet.notify_one();
        return ret_future;
    }
    void Wait();
    void Stop();
    std::size_t GetTaskSize();
    void JoinAll();
    bool IsStop() { return m_isStop; }

private:
    void terminate();
    void runTask();
    void cleanTaskQueue();
    std::vector<std::shared_ptr<std::thread>> m_threadVec;
    std::condition_variable m_taskGet;
    std::condition_variable m_taskEmpty;
    std::mutex m_mutex;
    std::queue<std::function<void(void)>> m_taskQueue;
    std::atomic<bool> m_isStop;
    std::once_flag m_callFlag;
    int m_taskSize;
};
}  // namespace utils

#endif  // ThreadPool_h__
