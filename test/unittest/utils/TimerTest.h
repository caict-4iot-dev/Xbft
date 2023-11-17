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
 * @file:   TimerTest.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/07/24  15:39:31
 */
#ifndef __TIMERTEST_H__
#define __TIMERTEST_H__
#include <gtest/gtest.h>
#include <stdio.h>
void GeneralFun(int &r_input) {
    r_input++;
};
class TimerTest : public testing::Test {
public:
    class TimerTestClass {
    public:
        static void StaicFun(int &r_input) { r_input++; };
        void MemberFun(int &r_input) { r_input++; };

    private:
    };
};

#endif  // TimerTest_h__