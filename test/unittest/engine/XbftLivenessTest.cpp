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
 * @date: 2023-11-17 17:46:38
 * @file: XbftLivenessTest.cpp
 */

#include "XbftLivenessTest.h"
#include "ConsEngine.h"
#include "Tool.h"
#include "XbftLiveness.h"

xbft::XbftEnvPointer XbftLivenessTest::CreateNewViewMsg(int64_t replicaId, int64_t viewNumber) {
    xbft::XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(0);
    p_xbft->set_type(protocol::XBFT_TYPE_NEWVIEW);
    protocol::XbftNewView *p_newView = p_xbft->mutable_new_view();
    p_newView->set_replica_id(replicaId);

    protocol::XbftTcValue *tcValue = p_newView->mutable_tc_content();

    tcValue->set_view_number(viewNumber);
    std::string pub = "pub-" + std::to_string(replicaId);
    std::string sig = "signature-" + std::to_string(replicaId);
    p_newView->mutable_signature()->set_public_key(pub);
    p_newView->mutable_signature()->set_sign_data(sig);

    p_env->mutable_signature()->set_public_key(pub);
    p_env->mutable_signature()->set_sign_data(sig);
    return p_env;
}

TEST_F(XbftLivenessTest, test) {
    auto net = Tool::CreateNet();
    ASSERT_TRUE(net != nullptr);

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    auto valueDeal = Tool::CreateValueDeal();
    ASSERT_TRUE(valueDeal != nullptr);

    auto keyInfo = Tool::CreateKeyInfo("test");
    ASSERT_TRUE(keyInfo != nullptr);

    auto nodeInfo = Tool::CreateNodeInfo(keyInfo, 1000);
    ASSERT_TRUE(nodeInfo != nullptr);

    auto consensus = std::make_shared<xbft::XbftConsensus>(net, valueDeal, keyTool);
    ASSERT_TRUE(consensus != nullptr);
    ASSERT_TRUE(consensus->Initialize(keyInfo, nodeInfo->GetValidators(), 0, ""));

    auto packMaker = std::make_shared<xbft::RoundRobinPaceMaker>(net, keyTool, consensus);
    ASSERT_TRUE(packMaker != nullptr);

    // Test error message
    protocol::XbftEnv errMsg;
    xbft::XbftMsgPointer msg = std::make_shared<xbft::XbftMsg>(errMsg, keyTool);
    ASSERT_FALSE(packMaker->OnRecv(msg));

    // Test tc value
    xbft::XbftEnvPointer msgTc = CreateNewViewMsg(1, 0);
    protocol::XbftTc tc;
    tc.add_high_qc_view_number(1);
    *(msgTc->mutable_xbft()->mutable_new_view()->mutable_last_tc()) = tc;
    ASSERT_FALSE(packMaker->OnRecv(std::make_shared<xbft::XbftMsg>(*msgTc, keyTool)));

    // Test error view number
    xbft::XbftEnvPointer msgTc1 = CreateNewViewMsg(1, 0);
    ASSERT_FALSE(packMaker->OnRecv(std::make_shared<xbft::XbftMsg>(*msgTc1, keyTool)));

    // duplicate receive
    xbft::XbftEnvPointer msgTc2 = CreateNewViewMsg(0, 1);
    ASSERT_TRUE(packMaker->OnRecv(std::make_shared<xbft::XbftMsg>(*msgTc2, keyTool)));
    ASSERT_FALSE(packMaker->OnRecv(std::make_shared<xbft::XbftMsg>(*msgTc2, keyTool)));
}