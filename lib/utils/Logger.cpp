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
 * @file: Logger.cpp
 */

#include "Logger.h"
#include <cstdarg>
#include <cstring>
#include <filesystem>

int utils::Logger::ms_logLevel = utils::LOG_LEVEL_TRACE;

// This function writes a prefix that matches glog's default format.
// The third parameter used to receive user-supplied data, and is NULL default
void utils::Logger::CustomerPrefix(std::ostream &r_os, const google::LogMessageInfo &cr_logMessageInfo, void *) {
    r_os << "[" << std::setw(4) << 1900 + cr_logMessageInfo.time.year() << '-' << std::setw(2)
         << 1 + cr_logMessageInfo.time.month() << '-' << std::setw(2) << cr_logMessageInfo.time.day() << ' '
         << std::setw(2) << cr_logMessageInfo.time.hour() << ':' << std::setw(2) << cr_logMessageInfo.time.min() << ':'
         << std::setw(2) << cr_logMessageInfo.time.sec() << "." << std::setw(6) << cr_logMessageInfo.time.usec() << ' '
         << std::setfill(' ') << std::setw(2) << cr_logMessageInfo.thread_id << std::setfill('0');
}

bool utils::Logger::InitializeGlog(const std::string &cr_fileName, int64_t expir, int64_t capacity,
    const std::string &cr_level, const std::string &cr_label) {
    int levelVlog = utils::LOG_LEVEL_TRACE;

    if (cr_level.find("TRACE") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_TRACE;
    else if (cr_level.find("DEBUG") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_DEBUG;
    else if (cr_level.find("INFO") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_INFO;
    else if (cr_level.find("PERFORMANCE") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_PERFORMANCE;
    else if (cr_level.find("WARNING") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_WARN;
    else if (cr_level.find("ERROR") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_ERROR;
    else if (cr_level.find("FATAL") != std::string::npos)
        levelVlog = utils::LOG_LEVEL_FATAL;

    // Is standard output required in addition to log files，default
    FLAGS_alsologtostderr = true;
    // default，When the color is not set at vlog level
    FLAGS_colorlogtostderr = true;
    // Set number of seconds that can buffer logs. 0 refers to real-time output
    FLAGS_logbufsecs = 0;
    // Log file size in MB
    FLAGS_max_log_size = capacity;
    FLAGS_stop_logging_if_full_disk = true;
    // keep your logs for n days,disable for google::DisableLogCleaner();
    google::EnableLogCleaner(expir);
    google::InstallFailureSignalHandler();
    // Initialize log instance
    google::InitGoogleLogging(cr_label.c_str(), &CustomerPrefix, nullptr);
    // Log configuration level
    FLAGS_v = levelVlog;
    utils::Logger::ms_logLevel = levelVlog;
    // Set log file path handle of vlog
    auto logPath = std::filesystem::path(cr_fileName).parent_path();
    if (!std::filesystem::exists(logPath)) {
        std::filesystem::create_directory(logPath);
    }
    google::SetLogDestination(google::GLOG_INFO, cr_fileName.c_str());
    // Set log file path handle of performance
#ifdef PERFORMANCE
    std::string fileTemp = cr_fileName + "PERFORMANCE";
    google::SetLogDestination(google::GLOG_WARNING, fileTemp.c_str());
#endif

    return true;
}
bool utils::Logger::Exit() {
    google::ShutdownGoogleLogging();
    return true;
}
void utils::Logger::SetLogLevel(int logLevel) {
    utils::Logger::ms_logLevel = logLevel;
}

int utils::Logger::LogGlogVm(const char *cp_file, const int c_lineNum, LogLevel logLevel, const char *cp_fmt, ...) {
    int retVal = 0;
    if (utils::Logger::ms_logLevel < logLevel) {
        return retVal;
    }
    va_list args1;
    va_start(args1, cp_fmt);
    va_list args2;
    va_copy(args2, args1);
    std::vector<char> lgBuffer(1 + std::vsnprintf(nullptr, 0, cp_fmt, args1));
    va_end(args1);

    std::vsnprintf(lgBuffer.data(), lgBuffer.size(), cp_fmt, args2);
    if (logLevel == utils::LOG_LEVEL_TRACE) {
        VLOG(utils::LOG_LEVEL_TRACE) << " TRACE" << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    } else if (logLevel == utils::LOG_LEVEL_DEBUG) {
        VLOG(utils::LOG_LEVEL_DEBUG) << " DEBUG" << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    } else if (logLevel == utils::LOG_LEVEL_INFO) {
        VLOG(utils::LOG_LEVEL_INFO) << " INFO " << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    } else if (logLevel == utils::LOG_LEVEL_WARN) {
        VLOG(utils::LOG_LEVEL_WARN) << " WARN " << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    } else if (logLevel == utils::LOG_LEVEL_ERROR) {
        VLOG(utils::LOG_LEVEL_ERROR) << " ERROR" << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    } else if (logLevel == utils::LOG_LEVEL_FATAL) {
        VLOG(utils::LOG_LEVEL_FATAL) << " FATAL" << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    } else if (logLevel == utils::LOG_LEVEL_PERFORMANCE) {
        // PERFORMANCE level is printed separately
        LOG(WARNING) << " PERFORMANCE" << ']' << ' ' << cp_file << ":" << c_lineNum << ' ' << lgBuffer.data();
    }
    va_end(args2);
    return retVal;
}
