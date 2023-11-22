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
 * @date: 2023-11-17 16:42:52
 * @file: XbftMsgTest.cpp
 */

#include "XbftMsg.h"
#include "ConsEngine.h"
#include "Tool.h"
#include "XbftMsgTest.h"


xbft::XbftEnvPointer XbftMsgTest::CreateNewViewMsg(int64_t replicaId, int64_t viewNumber) {
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

TEST_F(XbftMsgTest, MsgCheck) {
    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    xbft::XbftEnvPointer xbftEnv = CreateNewViewMsg(0, 0);
    xbft::XbftMsg msg1(*xbftEnv, keyTool);
    xbft::XbftMsg msg2(*xbftEnv, keyTool);

    ASSERT_TRUE(msg1 == msg2);
    ASSERT_FALSE(msg1 < msg2);
    ASSERT_EQ(msg1.GetStrType(), "NEWVIEW");
    ASSERT_TRUE(msg1.GetSize() > 0);

    xbft::XbftValidatorSet validators;
    validators.Update("address-0", 0);
    ASSERT_TRUE(msg1.CheckMsgItem(validators));

    // // check has not xbft object
    xbft::XbftValidatorSet validators2;
    validators2.Update("address-0", 0);
    protocol::XbftEnv env;
    env.mutable_signature()->set_public_key("pub-0");
    env.mutable_signature()->set_sign_data("sig-0");
    xbft::XbftMsg msg3(env, keyTool);
    ASSERT_FALSE(msg3.CheckMsgItem(validators2));

    // check propose
    protocol::XbftEnv envPropose;
    envPropose.mutable_xbft()->set_type(protocol::XBFT_TYPE_PROPOSE);
    envPropose.mutable_signature()->set_public_key("pub-0");
    envPropose.mutable_signature()->set_sign_data("sig-0");
    xbft::XbftMsg msg4(envPropose, keyTool);
    ASSERT_FALSE(msg4.CheckMsgItem(validators2));

    // check Vote
    protocol::XbftEnv envVote;
    envVote.mutable_xbft()->set_type(protocol::XBFT_TYPE_VOTE);
    envVote.mutable_signature()->set_public_key("pub-0");
    envVote.mutable_signature()->set_sign_data("sig-0");
    xbft::XbftMsg msg5(envVote, keyTool);
    ASSERT_FALSE(msg5.CheckMsgItem(validators2));

    // check newview
    protocol::XbftEnv envNewView;
    envNewView.mutable_xbft()->set_type(protocol::XBFT_TYPE_NEWVIEW);
    envNewView.mutable_signature()->set_public_key("pub-0");
    envNewView.mutable_signature()->set_sign_data("sig-0");
    xbft::XbftMsg msg6(envNewView, keyTool);
    ASSERT_FALSE(msg6.CheckMsgItem(validators2));

    // check right
    protocol::XbftEnv envProposeRight;
    envProposeRight.mutable_xbft()->set_type(protocol::XBFT_TYPE_PROPOSE);
    envProposeRight.mutable_xbft()->mutable_propose()->set_replica_id(0);
    envProposeRight.mutable_xbft()->mutable_propose()->mutable_node()->set_view_number(0);
    envProposeRight.mutable_signature()->set_public_key("pub-0");
    envProposeRight.mutable_signature()->set_sign_data("sig-0");
    xbft::XbftMsg msg8(envProposeRight, keyTool);
    ASSERT_TRUE(msg8.CheckMsgItem(validators2));
}
