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
 * @file:   ExitHandler.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/06/15  16:52:36
 */
#ifndef __EXITHANDLER_H__
#define __EXITHANDLER_H__
#include "Logger.h"
#include <atomic>
#include <csignal>

namespace common {

class ExitHandler {
public:
    static void ResgisterSignal() {
        struct sigaction signal {};
        signal.sa_handler = ExitSignalHandler;
        sigemptyset(&signal.sa_mask);
        signal.sa_flags = 0;
        sigaction(SIGINT, &signal, nullptr);   // 2
        sigaction(SIGTERM, &signal, nullptr);  // 15
        sigaction(SIGQUIT, &signal, nullptr);  // 3
        sigaction(SIGHUP, &signal, nullptr);   // 1
    }
    static void ExitSignalHandler(int sig) {
        ms_exitFlag = true;
        LOG_INFO(
            "Got a exit signal, signal number:%d,the program will exit "
            "right now!",
            sig);
        std::cout << "Got a exit signal, signal number:" << sig << ",the program will exit right now" << std::endl;
    }
    static std::atomic_bool GetExitFlag() { return ms_exitFlag.load(); };
    static std::atomic_bool ms_exitFlag;
};
}  // namespace common

#endif  // ExitHandler_h__