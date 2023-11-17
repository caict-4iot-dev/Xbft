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
 * @date: 2023-11-17 09:49:05
 * @file: LoggerTest.cpp
 */


#include "LoggerTest.h"
#include "Logger.h"
#include <filesystem>

TEST_F(LoggerTest, InitializeGlog) {
    std::string path = "./testlog/";
    utils::Logger::InitializeGlog(path, utils::LOG_LEVEL_INFO, "xbft");
    ASSERT_EQ(utils::Logger::ms_logLevel, utils::LOG_LEVEL_INFO);
    LOG_INFO("this is test line");
    EXPECT_TRUE(std::filesystem::exists(path));
    utils::Logger::Exit();
    std::filesystem::remove_all(path);
}

TEST_F(LoggerTest, SetLogLevel) {
    int log_level = 4;
    utils::Logger::SetLogLevel(log_level);
    ASSERT_EQ(utils::Logger::ms_logLevel, log_level);
}
