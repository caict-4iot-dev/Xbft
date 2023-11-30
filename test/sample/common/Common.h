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
 * @date: 2023-11-27 17:56:34
 * @file: Common.h
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <vector>

namespace common {

const int QUEUE_LOOP_TIME = 100;
const int TIMER_LOOP_TIME = 1000;
const int NET_RECV_BUFF_SIZE = 50*1024;

typedef void (*SendMsgFun)(
    const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value);

typedef void (*SendMsgTypeFun)(
    const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value, bool request);

struct Msg {
    std::string crValue;
    std::vector<std::string> crDest;
};

struct SyncMsg {
    bool request;
    std::string crValue;
    std::string crDest;
};

}  // namespace common

#endif
