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
 * @date: 2023-11-22 10:08:38
 * @file: XbftTest.cpp
 */

#include "XbftTest.h"
#include "ConsEngine.h"
#include "Tool.h"
#include "Xbft.h"


TEST_F(XbftTest, Create) {
    auto net = Tool::CreateNet();
    auto keyTool = Tool::CreateKeyTool();
    auto valueDeal = Tool::CreateValueDeal();
    auto key = Tool::CreateKeyInfo("test");
    auto nodeInfo = Tool::CreateNodeInfo(key, 100);

    auto engine = xbft::CreateXbftEngine(net, valueDeal, nodeInfo, keyTool);
    ASSERT_TRUE(engine != nullptr);

    EXPECT_EQ(engine->GetEngineName(), "Xbft Engine");
    EXPECT_EQ(engine->GetEngineVersion(), "1.0.0");

    EXPECT_EQ(IsLeader(engine), false);
    EXPECT_EQ(GetViewNumber(engine), 0);
    EXPECT_EQ(IsViewActive(engine), true);
}