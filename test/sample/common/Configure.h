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
 * @date: 2023-11-22 15:34:26
 * @file: Configure.h
 */

#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

namespace common {

class Configure {
public:
    static bool LoadConfig(const std::string &cr_cfgPath);
    static void Print(YAML::Node config);
};

class NodeConfig {
public:
    static bool LoadConfig(const YAML::Node &cr_config);

public:
    static std::vector<std::string> m_priKeys;
    static int64_t m_interval;
};

class ConsConfig {
public:
    static bool LoadConfig(const YAML::Node &cr_config);

public:
    static std::vector<std::string> m_validators;
    static int64_t m_interval;
};

class NetConfig {
public:
    static bool LoadConfig(const YAML::Node &cr_config);
};

class LogConfig {
public:
    static bool LoadConfig(const YAML::Node &cr_config);

public:
    static int ms_expireDays;     // Expiration time, by day
    static std::string ms_level;  // log level
    static int ms_sizeCapacity;   // log file size of capacity
    static std::string ms_path;   // log file path
};

}  // namespace common
#endif
