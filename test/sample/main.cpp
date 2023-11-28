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
 * @date: 2023-11-22 18:25:51
 * @file: main.cpp
 */

#include "BlockChain.h"
#include "Configure.h"
#include "ConsEngine.h"
#include "Ed25519.h"
#include "ExitHandler.h"
#include "Logger.h"
#include "Network.h"
#include "Singleton.h"
#include "utils/Strings.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>


std::atomic_bool common::ExitHandler::ms_exitFlag = false;

void GenerateKey() {
    auto key = std::make_shared<common::Ed25519>();
    auto pri = key->GetPrivateKey();
    auto pub = key->GetPublicKey();
    auto address = common::PublicKeyToAddress(pub);
    std::cout << "[" << std::endl;
    std::cout << "    pri:" << pri << std::endl;
    std::cout << "    pub:" << pub << std::endl;
    std::cout << "    address:" << address << std::endl;
    std::cout << "]" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        // 添加ed25519生成私钥地址小工具
        std::string cmd = argv[1];
        if (cmd == "key") {
            GenerateKey();
        }
        return 0;
    }

    // 加载配置
    std::string filename = "./config.yaml";
    std::ifstream file(filename);
    if (!file) {
        std::cout << "please check config.yaml file" << std::endl;
        return -1;
    }

    if (!common::Configure::LoadConfig(filename)) {
        return -1;
    }
    if (!utils::Logger::InitializeGlog(common::LogConfig::ms_path, common::LogConfig::ms_expireDays,
            common::LogConfig::ms_sizeCapacity, common::LogConfig::ms_level, "Xbft-sample")) {
        return -1;
    };

    common::ExitHandler::ResgisterSignal();

    net::Network::InitInstance();
    dealing::BlockChain::InitInstance();

    do {
        // 启动网络模块
        net::Network &network = net::Network::Instance();
        if (!network.Initialize()) {
            LOG_ERROR("Failed to initialize net");
            break;
        }

        // 启动数据处理模块
        dealing::BlockChain &blockChain = dealing::BlockChain::Instance();
        if (!blockChain.Initialize(
                network.sendCons, network.sendSync, network.GetMsgQueue(), network.GetSyncMsgQueue())) {
            LOG_ERROR("Failed to initialize blockChain");
            break;
        }

        while (!common::ExitHandler::GetExitFlag()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        blockChain.Exit();
        network.Exit();

    } while (false);

    dealing::BlockChain::ExitInstance();
    net::Network::ExitInstance();
    utils::Logger::Exit();

    return 0;
}