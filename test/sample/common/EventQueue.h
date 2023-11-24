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
 * @file:   EventQueue.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/05/10  16:49:44
 */
#ifndef __EVENTQUEUE_H__
#define __EVENTQUEUE_H__
#include <atomic>
#include <condition_variable>
#include <queue>

namespace common {
constexpr int MAX_QUEUE_SIZE = 10240;
constexpr int DEFALT_QUEUE_SIZE = 256;
template <typename T>
class EventQueue {
public:
    EventQueue(int capacity = DEFALT_QUEUE_SIZE) {
        capacity < MAX_QUEUE_SIZE ? m_capacity = capacity : m_capacity = MAX_QUEUE_SIZE;
        m_size = 0;
    };                                                    // construct
    ~EventQueue() = default;                              // destruct
    EventQueue(const EventQueue &a) = delete;             // copy construct
    EventQueue(EventQueue &&) = delete;                   // move construct
    EventQueue &operator=(const EventQueue &a) = delete;  // assign
    EventQueue &operator=(EventQueue &&) = delete;        // move
    template <typename Args>
    inline void Push(Args &&value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_notifyPut.wait(lock, [this]() { return m_size < m_capacity; });
        m_queue.emplace(T(std::forward<Args>(value)));
        m_size++;
        m_notifyGet.notify_one();
    };
    template <typename _Rep, typename _Period>
    inline bool TryPop(const std::chrono::duration<_Rep, _Period> &cr_duration, T &r_value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_notifyGet.wait_for(lock, cr_duration, [this]() { return m_size > 0; })) {
            return false;
        }
        r_value = std::move(m_queue.front());
        m_queue.pop();
        m_size--;
        m_notifyPut.notify_one();
        return true;
    }
    inline T Pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_notifyGet.wait(lock, [this]() { return m_size > 0; });
        T value = std::move(m_queue.front());
        m_queue.pop();
        m_size--;
        m_notifyPut.notify_one();
        return value;
    };
    inline void Clear() {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::queue<T>().swap(m_queue);
        m_size = 0;
        m_notifyPut.notify_all();
    };
    inline size_t Size() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_size;
    };
    inline bool IsEmpty() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_size == 0;
    };
    inline bool IsFull() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_size == m_capacity;
    };
    inline size_t GetCapacity() { return m_capacity; };

private:
    size_t m_capacity;
    size_t m_size;
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_notifyPut;
    std::condition_variable m_notifyGet;
};

}  // namespace common

#endif  // EventQueue_h__