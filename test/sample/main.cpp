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

#include "Configure.h"
#include "Singleton.h"
#include "StoreValue.h"
#include "ThreadPool.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        // 添加ed25519生成私钥地址小工具
        return 0;
    }

    // 加载配置
    if (!common::Configure::LoadConfig("./config/config.yaml")) {
        return -1;
    }
    if (!utils::Logger::InitializeGlog(common::LogConfig::ms_path, common::LogConfig::ms_level, "Xbft-sample")) {
        return -1;
    };

    // 启动网络模块

    // 启动数据处理模块
    dealing::StoreValue::InitInstance();

    // 启动业务线程 根据node个数启动对应个数的线程
    size_t nodeNum = common::NodeConfig::m_priKeys.size();
    auto nodePool = std::make_shared<common::ThreadPool>();
    if (nodePool == nullptr) {
        return -1;
    }
    nodePool->InitThreadNum(nodeNum);

    //添加任务

    while (nodePool->GetSize() == nodeNum) {
        // 检测业务线程异常时退出循环
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 3));
    }

    dealing::StoreValue::ExitInstance();

    return 0;
}