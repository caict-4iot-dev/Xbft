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

 * @brief: for ThreadPool and Timer
 * @file:   FunctionWraper.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/06/18  16:46:30
 */
#ifndef __FUNCTIONWRAPER_H__
#define __FUNCTIONWRAPER_H__
#include <functional>
#include <future>
#include <memory>

namespace utils {

template <typename Return>
struct FuncWrapper {
    template <typename Function, typename... Args>
    auto GetWrapped(Function &&rr_func, std::shared_ptr<std::promise<Return>> p_promise, Args &&...args) {
        return [p_promise, rr_func, args...]() mutable {
            p_promise->set_value(std::invoke(rr_func, std::forward<Args>(args)...));
        };
    }
};
template <>
struct FuncWrapper<void> {
    template <typename Function, typename... Args>
    auto GetWrapped(Function &&rr_func, std::shared_ptr<std::promise<void>> p_promise, Args &&...args) {
        return [p_promise, rr_func, args...]() mutable {
            std::invoke(rr_func, std::forward<Args>(args)...);
            p_promise->set_value();
        };
    }
};
}  // namespace utils
#endif  // FunctionWraper_h__