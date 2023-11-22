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
 * @date: 2023-11-20 17:03:49
 * @file: XbftConsensusTest.cpp
 */

#include "XbftConsensusTest.h"
#include "ConsEngine.h"
#include "Tool.h"
#include "XbftConsensus.h"


xbft::XbftEnvPointer XbftConsensusTest::CreateProposeMsg(int64_t replicaId, int64_t viewNumber) {
    xbft::XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(0);
    p_xbft->set_type(protocol::XBFT_TYPE_PROPOSE);
    protocol::XbftPropose *p_propose = p_xbft->mutable_propose();
    p_propose->set_replica_id(replicaId);
    p_propose->mutable_node()->set_view_number(viewNumber);
    p_propose->mutable_node()->set_sequence(2);
    p_propose->mutable_node()->set_node_value("test value");

    std::string pub = "pub-" + std::to_string(replicaId);
    std::string sig = "sig-" + std::to_string(replicaId);

    p_env->mutable_signature()->set_public_key(pub);
    p_env->mutable_signature()->set_sign_data(sig);

    return p_env;
}

xbft::XbftEnvPointer XbftConsensusTest::CreateVoteMsg(int64_t replicaId, int64_t viewNumber, int64_t sequence) {
    xbft::XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(0);
    p_xbft->set_type(protocol::XBFT_TYPE_VOTE);
    protocol::XbftVote *p_vote = p_xbft->mutable_vote();
    p_vote->set_replica_id(replicaId);
    protocol::XbftQcValue *qcValue = p_vote->mutable_qc_content();

    std::string node_hash = "";
    qcValue->set_node_hash(node_hash);
    qcValue->set_node_value_hash("");
    qcValue->set_view_number(viewNumber);
    qcValue->set_sequence(sequence);

    std::string pub = "pub-" + std::to_string(replicaId);
    std::string sig = "sig-" + std::to_string(replicaId);

    p_vote->mutable_signature()->set_public_key(pub);
    p_vote->mutable_signature()->set_sign_data(sig);

    p_env->mutable_signature()->set_public_key(pub);
    p_env->mutable_signature()->set_sign_data(sig);
    return p_env;
}

TEST_F(XbftConsensusTest, InValidProof) {
    auto net = Tool::CreateNet();
    ASSERT_TRUE(net != nullptr);

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    auto valueDeal = Tool::CreateValueDeal();
    ASSERT_TRUE(valueDeal != nullptr);

    auto keyInfo = Tool::CreateKeyInfo("test");
    ASSERT_TRUE(keyInfo != nullptr);

    auto nodeInfo = Tool::CreateNodeInfo(keyInfo, 100);
    ASSERT_TRUE(nodeInfo != nullptr);

    auto cons = std::make_shared<xbft::XbftConsensus>(net, valueDeal, keyTool);
    ASSERT_TRUE(cons != nullptr);

    ASSERT_FALSE(cons->Initialize(keyInfo, nodeInfo->GetValidators(), 0, "123"));
}

TEST_F(XbftConsensusTest, XbftProofCheck) {
    auto net = Tool::CreateNet();
    ASSERT_TRUE(net != nullptr);

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    auto valueDeal = Tool::CreateValueDeal();
    ASSERT_TRUE(valueDeal != nullptr);

    auto keyInfo = Tool::CreateKeyInfo("test");
    ASSERT_TRUE(keyInfo != nullptr);

    auto nodeInfo = Tool::CreateNodeInfo(keyInfo, 100);
    ASSERT_TRUE(nodeInfo != nullptr);

    auto cons = std::make_shared<xbft::XbftConsensus>(net, valueDeal, keyTool);
    ASSERT_TRUE(cons != nullptr);
    ASSERT_TRUE(cons->Initialize(keyInfo, nodeInfo->GetValidators(), 0, ""));

    // Test proof has not qc content
    protocol::XbftProof proofNoQc;
    proofNoQc.mutable_node()->set_view_number(1);
    const protocol::XbftQc &xbftQc = proofNoQc.qc();
    ASSERT_FALSE(xbftQc.has_qc_content());
    ASSERT_FALSE(cons->UpdateProof(proofNoQc.SerializeAsString()));

    // Test update proof
    protocol::XbftProof proofQc;
    proofQc.mutable_qc()->mutable_qc_content()->set_view_number(1);
    proofQc.mutable_qc()->mutable_qc_content()->set_sequence(2);
    ASSERT_TRUE(cons->UpdateProof(proofQc.SerializeAsString()));
}

TEST_F(XbftConsensusTest, XbftMsgCheck) {
    auto net = Tool::CreateNet();
    ASSERT_TRUE(net != nullptr);

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    auto valueDeal = Tool::CreateValueDeal();
    ASSERT_TRUE(valueDeal != nullptr);

    auto keyInfo = Tool::CreateKeyInfo("0");
    ASSERT_TRUE(keyInfo != nullptr);

    auto nodeInfo = Tool::CreateNodeInfo(keyInfo, 100);
    ASSERT_TRUE(nodeInfo != nullptr);

    auto cons = std::make_shared<xbft::XbftConsensus>(net, valueDeal, keyTool);
    ASSERT_TRUE(cons != nullptr);
    ASSERT_TRUE(cons->Initialize(keyInfo, nodeInfo->GetValidators(), 0, ""));

    // Test error message CheckMsgItem error
    protocol::XbftEnv errMsg;
    xbft::XbftMsgPointer msg = std::make_shared<xbft::XbftMsg>(errMsg, keyTool);
    ASSERT_FALSE(cons->OnRecv(msg));

    // Test view not active
    cons->SetViewInActive();
    ASSERT_TRUE(cons->IsViewActive() == false);
    xbft::XbftEnvPointer msg1 = CreateProposeMsg(1, 0);
    ASSERT_FALSE(cons->OnRecv(std::make_shared<xbft::XbftMsg>(*msg1, keyTool)));

    // Test Message view >= local view number
    xbft::XbftQcPointer qcp = nullptr;
    cons->ViewChanged(1, qcp);
    xbft::XbftEnvPointer msg2 = CreateProposeMsg(0, 0);
    ASSERT_FALSE(cons->OnRecv(std::make_shared<xbft::XbftMsg>(*msg2, keyTool)));
}

TEST_F(XbftConsensusTest, Check) {
    // Test propose message, Received PROPOSE message, leader would not process
    auto net = Tool::CreateNet();
    ASSERT_TRUE(net != nullptr);

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    auto valueDeal = Tool::CreateValueDeal();
    ASSERT_TRUE(valueDeal != nullptr);

    auto keyInfo = Tool::CreateKeyInfo("0");
    ASSERT_TRUE(keyInfo != nullptr);

    auto nodeInfo = Tool::CreateNodeInfo(keyInfo, 100);
    ASSERT_TRUE(nodeInfo != nullptr);

    auto cons = std::make_shared<xbft::XbftConsensus>(net, valueDeal, keyTool);
    ASSERT_TRUE(cons != nullptr);
    ASSERT_TRUE(cons->Initialize(keyInfo, nodeInfo->GetValidators(), 0, ""));

    xbft::XbftEnvPointer msg1 = CreateProposeMsg(0, 0);
    ASSERT_TRUE(cons->OnRecv(std::make_shared<xbft::XbftMsg>(*msg1, keyTool)));

    // Test propose message
    ASSERT_TRUE(cons->Initialize(keyInfo, nodeInfo->GetValidators(), 1, ""));
    xbft::XbftEnvPointer msg2 = CreateProposeMsg(2, 1);
    ASSERT_FALSE(cons->OnRecv(std::make_shared<xbft::XbftMsg>(*msg2, keyTool)));

    // Test vote message, Received VOTE message, but replica would not process
    ASSERT_TRUE(cons->Initialize(keyInfo, nodeInfo->GetValidators(), 1, ""));
    xbft::XbftEnvPointer msg3 = CreateVoteMsg(0, 1, 1);
    ASSERT_TRUE(cons->OnRecv(std::make_shared<xbft::XbftMsg>(*msg3, keyTool)));
}