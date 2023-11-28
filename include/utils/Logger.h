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
 * @date: 2023-11-15 15:30:31
 * @file: Logger.h
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <vector>



#define FILENAME(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x

#define LOG_TRACE(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#ifdef PERFORMANCE
#define LOG_PERFORMANCE(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_PERFORMANCE, fmt, ##__VA_ARGS__)

#else
#define LOG_PERFORMANCE(fmt, ...) \
    {}
#endif  // PERFORMANCE

#define LOG_TRACE_ERRNO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_TRACE, fmt " (%u:%s)", ##__VA_ARGS__)
#define LOG_DEBUG_ERRNO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_DEBUG, fmt " (%u:%s)", ##__VA_ARGS__)
#define LOG_INFO_ERRNO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_INFO, fmt " (%u:%s)", ##__VA_ARGS__)
#define LOG_WARN_ERRNO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_WARN, fmt " (%u:%s)", ##__VA_ARGS__)
#define LOG_ERROR_ERRNO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_ERROR, fmt " (%u:%s)", ##__VA_ARGS__)
#define LOG_FATAL_ERRNO(fmt, ...) \
    utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_FATAL, fmt " (%u:%s)", ##__VA_ARGS__)

#define PROCESS_EXIT(fmt, ...)                                                                              \
    {                                                                                                       \
        utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__); \
        exit(-1);                                                                                           \
    }
#define PROCESS_EXIT_ERRNO(fmt, ...)                                                                                   \
    {                                                                                                                  \
        utils::Logger::LogGlogVm(FILENAME(__FILE__), __LINE__, utils::LOG_LEVEL_ERROR, fmt " (%u:%s)", ##__VA_ARGS__); \
        exit(-1);                                                                                                      \
    }
#define LOG_BUF_SIZE 1024

namespace utils {
enum LogLevel {
    LOG_LEVEL_TRACE = 6,
    LOG_LEVEL_DEBUG = 5,
    LOG_LEVEL_INFO = 4,
    LOG_LEVEL_PERFORMANCE = 3,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_FATAL = 0,
};

class Logger {
public:
    Logger() = delete;                            // Constructor
    ~Logger() = delete;                           // Destructor
    Logger(const Logger &a) = delete;             // copy constructor
    Logger(Logger &&) = delete;                   // move constructor
    Logger &operator=(const Logger &a) = delete;  // Copy assignment function
    Logger &operator=(Logger &&) = delete;        // Move assignment function
    static bool InitializeGlog(const std::filesystem::path &cr_fileName, int64_t expir, int64_t capacity,
        const std::string &cr_level, const std::string &cr_label);
    static bool Exit();
    static void CustomerPrefix(std::ostream &r_os, const google::LogMessageInfo &cr_logMessageInfo, void *);
    static int LogGlogVm(const char *cp_file, const int c_lineNum, LogLevel log_Level, const char *cp_fmt, ...);
    static void SetLogLevel(int logLevel);

public:
    static int ms_logLevel;
};
}  // namespace utils
#endif  // Logger_h__
