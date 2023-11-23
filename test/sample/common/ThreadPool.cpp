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
 * @file:   ThreadPool.cpp
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/08/04  0:56:32
 */
#include "ThreadPool.h"

bool common::ThreadPool::InitThreadNum(std::size_t num, bool bindCpu) {
    if (num <= 0) {
        num = std::thread::hardware_concurrency();
    } else if (num > MAX_SIZE) {
        num = MAX_SIZE;
    }
    for (std::size_t i = 0; i < num; i++) {
        std::shared_ptr<std::thread> t = std::make_shared<std::thread>([this, bindCpu, i] {
            if (bindCpu) {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(i, &cpuset);
                pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            }
            this->runTask();
        });
        m_threadVec.emplace_back(t);
    }
    return true;
}

std::size_t common::ThreadPool::GetSize() {
    return m_threadVec.size();
}

void common::ThreadPool::Wait() {
    std::unique_lock<std::mutex> locker(m_mutex);
    if (m_taskSize > 0) {
        m_taskEmpty.wait(locker, [this] { return m_taskSize == 0; });
    }
}

void common::ThreadPool::Stop() {
    std::call_once(m_callFlag, [this] { terminate(); });
}

std::size_t common::ThreadPool::GetTaskSize() {
    std::unique_lock<std::mutex> locker(m_mutex);
    return m_taskSize;
}

void common::ThreadPool::JoinAll() {
    for (auto &iter : m_threadVec) {
        if (iter != nullptr && iter->joinable()) {
            iter->join();
        }
    }
}

void common::ThreadPool::terminate() {
    std::unique_lock<std::mutex> locker(m_mutex);
    m_isStop = true;
    m_taskGet.notify_all();
    m_taskEmpty.notify_all();
    locker.unlock();
    JoinAll();
    m_taskSize = 0;
    m_threadVec.clear();
    cleanTaskQueue();
}

void common::ThreadPool::runTask() {
    while (true) {
        std::function<void()> task = nullptr;

        std::unique_lock<std::mutex> locker(m_mutex);

        while (m_taskQueue.empty() && !m_isStop) {
            m_taskGet.wait(locker);
        }

        if (m_isStop) {
            break;
        }

        if (!m_taskQueue.empty()) {
            task = std::move(m_taskQueue.front());
            m_taskQueue.pop();
        }
        locker.unlock();

        if (task != nullptr) {
            task();
            locker.lock();
            m_taskSize--;
            locker.unlock();
        }
        if (m_taskSize == 0) {
            m_taskEmpty.notify_all();
        }
    }
}

void common::ThreadPool::cleanTaskQueue() {
    while (!m_taskQueue.empty()) {
        m_taskQueue.pop();
    }
}
