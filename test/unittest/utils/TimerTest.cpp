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
 * @file:   TimerTest.cpp
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/08/04  0:50:20
 */

#include "TimerTest.h"
#include "Timer.h"
/*
1. 普通函数timeout
2. 普通函数until
3. 普通函数loop
4. 类静态函数timeout
5. 类静态函数until
6. 类静态函数loop
7. lamada函数timeout
8. lamada函数until
9. lamada函数loop
10. 类成员函数timeout
11. 类成员函数until
12. 类成员函数loop
13. timeout stop
14. until stop
15. loop stop
*/
TEST_F(TimerTest, GenFunTimeFor) {
    utils::TimerFor timer;
    int num = 0;
    timer.SetTimeOut(20, GeneralFun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, GenFunTimeUntil) {
    utils::TimerUntil timer;
    int num = 0;
    auto now = std::chrono::system_clock::now();
    timer.SetTimeOut(now + std::chrono::milliseconds(20), GeneralFun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, GenFunTimeLoop) {
    utils::TimerLoop timer;
    int num = 0;
    timer.SetTimeOut(30, GeneralFun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Stop();
    ASSERT_EQ(num, 3);
}

TEST_F(TimerTest, StaticMemFunTimeFor) {
    utils::TimerFor timer;
    int num = 0;
    timer.SetTimeOut(20, TimerTestClass::StaicFun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, StaticMemFunTimeUntil) {
    utils::TimerUntil timer;
    int num = 0;
    auto now = std::chrono::system_clock::now();
    timer.SetTimeOut(now + std::chrono::milliseconds(20), TimerTestClass::StaicFun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, StaticMemFunTimeLoop) {
    utils::TimerLoop timer;
    int num = 0;
    timer.SetTimeOut(30, TimerTestClass::StaicFun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Stop();
    ASSERT_EQ(num, 3);
}

TEST_F(TimerTest, MemFunTimeFor) {
    utils::TimerFor timer;
    int num = 0;
    TimerTestClass test;
    timer.SetTimeOut(20, &TimerTestClass::MemberFun, &test, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, MemFunTimeUntil) {
    utils::TimerUntil timer;
    int num = 0;
    TimerTestClass test;
    auto now = std::chrono::system_clock::now();
    timer.SetTimeOut(now + std::chrono::milliseconds(20), &TimerTestClass::MemberFun, &test, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, MemFunTimeLoop) {
    utils::TimerLoop timer;
    int num = 0;
    TimerTestClass test;
    timer.SetTimeOut(30, &TimerTestClass::MemberFun, &test, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Stop();
    ASSERT_EQ(num, 3);
}

TEST_F(TimerTest, LamadaFunTimeFor) {
    utils::TimerFor timer;
    int num = 0;
    auto fun = [](int &r_input) { r_input++; };
    timer.SetTimeOut(20, fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, LamadaFunTimeUntil) {
    utils::TimerUntil timer;
    int num = 0;
    auto fun = [](int &r_input) { r_input++; };
    auto now = std::chrono::system_clock::now();
    timer.SetTimeOut(now + std::chrono::milliseconds(20), fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
}
TEST_F(TimerTest, LamadaFunTimeLoop) {
    utils::TimerLoop timer;
    int num = 0;
    auto fun = [](int &r_input) { r_input++; };
    timer.SetTimeOut(30, fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Stop();
    ASSERT_EQ(num, 3);
}

TEST_F(TimerTest, TimeForStop) {
    utils::TimerFor timer;
    int num = 0;
    auto fun = [](int &r_input) { r_input++; };
    timer.SetTimeOut(20, fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
    timer.Stop();
    timer.SetTimeOut(20, fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 2);
}
TEST_F(TimerTest, TimeUntilStop) {
    utils::TimerUntil timer;
    int num = 0;
    auto fun = [](int &r_input) { r_input++; };
    auto now = std::chrono::system_clock::now();
    timer.SetTimeOut(now + std::chrono::milliseconds(20), fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 1);
    timer.Stop();
    now = std::chrono::system_clock::now();
    timer.SetTimeOut(now + std::chrono::milliseconds(20), fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_EQ(num, 2);
    timer.Stop();
}
TEST_F(TimerTest, TimeLoopStop) {
    utils::TimerLoop timer;
    int num = 0;
    auto fun = [](int &r_input) { r_input++; };
    timer.SetTimeOut(30, fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Stop();
    ASSERT_EQ(num, 3);
    timer.SetTimeOut(30, fun, std::ref(num));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Stop();
    ASSERT_EQ(num, 6);
}