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
 * @file: Configure.cpp
 */

#include "Configure.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace common {

bool Configure::LoadConfig(const std::string &cr_cfgPath) {
    do {
        YAML::Node config = YAML::LoadFile(cr_cfgPath);
        // load node config
        if (!NodeConfig::LoadConfig(config)) {
            std::cout << "Load node config failed." << std::endl;
            break;
        }

        // load cons config
        if (!ConsConfig::LoadConfig(config)) {
            std::cout << "Load cons config failed." << std::endl;
            break;
        }

        // load logger config
        if (!LogConfig::LoadConfig(config)) {
            std::cout << "Load logger config failed." << std::endl;
            break;
        }

        return true;
    } while (false);
    return false;
}

void Configure::Print(YAML::Node config) {
    for (auto it : config)
        std::cout << it.first << it.second << std::endl;
}

std::vector<std::string> NodeConfig::m_priKeys;
int64_t NodeConfig::m_interval;

bool NodeConfig::LoadConfig(const YAML::Node &cr_config) {
    bool ret = true;
    try {
        // load logger configure
        auto pri0 = cr_config["node"]["node-0"]["private_key"].as<std::string>();
        NodeConfig::m_priKeys.push_back(pri0);
        auto pri1 = cr_config["node"]["node-1"]["private_key"].as<std::string>();
        NodeConfig::m_priKeys.push_back(pri1);
        auto pri2 = cr_config["node"]["node-2"]["private_key"].as<std::string>();
        NodeConfig::m_priKeys.push_back(pri2);
        auto pri3 = cr_config["node"]["node-3"]["private_key"].as<std::string>();
        NodeConfig::m_priKeys.push_back(pri3);

        NodeConfig::m_interval = cr_config["node"]["interval"].as<int>();

        std::cout << "Load Node configure successful." << std::endl;
    } catch (const std::exception &e) {
        ret = false;
    }
    return ret;
}

std::vector<std::string> ConsConfig::m_validators;
int64_t ConsConfig::m_interval;

bool ConsConfig::LoadConfig(const YAML::Node &cr_config) {
    bool ret = true;
    try {
        // load logger configure
        for (auto &it : cr_config["consensus"]["validators"]) {
            ConsConfig::m_validators.push_back(it.as<std::string>());
        }
        ConsConfig::m_interval = cr_config["consensus"]["interval"].as<int>();

        std::cout << "Load consensus configure successful." << std::endl;
    } catch (const std::exception &e) {
        ret = false;
    }
    return ret;
}

int LogConfig::ms_expireDays = 10;
int LogConfig::ms_sizeCapacity = 10;
std::string LogConfig::ms_level = "";
std::string LogConfig::ms_path = "";

bool LogConfig::LoadConfig(const YAML::Node &cr_config) {
    bool ret = true;
    try {
        // load logger configure
        LogConfig::ms_expireDays = cr_config["logger"]["expire_days"].as<int>();
        LogConfig::ms_level = cr_config["logger"]["level"].as<std::string>();
        LogConfig::ms_sizeCapacity = cr_config["logger"]["size_capacity"].as<int>();
        LogConfig::ms_path = cr_config["logger"]["path"].as<std::string>();

        std::cout << "Load logger configure successful." << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Load logger config exception, detail is " << e.what() << std::endl;
        ret = false;
    }
    return ret;
}

}  // namespace common
