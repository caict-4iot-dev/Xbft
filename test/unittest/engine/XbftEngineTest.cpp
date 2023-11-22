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
 * @date: 2023-11-20 18:19:52
 * @file: XbftEngineTest.cpp
 */

#include "XbftEngineTest.h"

std::shared_ptr<xbft::XbftEngine> XbftEngineTest::CreateXbftEngine(int64_t interVal, std::shared_ptr<Key> p_key) {
    auto net = Tool::CreateNet();
    auto keyTool = Tool::CreateKeyTool();
    auto valueDeal = Tool::CreateValueDeal();
    auto nodeInfo = Tool::CreateNodeInfo(p_key, interVal);

    auto engine = std::make_shared<xbft::XbftEngine>(net, valueDeal, keyTool);
    engine->StartEngine(nodeInfo);

    return engine;
}

TEST_F(XbftEngineTest, base) {
    const int64_t sendInterval = 10000000;
    std::shared_ptr<Key> keyInfo = Tool::CreateKeyInfo("test");
    ASSERT_TRUE(keyInfo != nullptr);

    auto engine = XbftEngineTest::CreateXbftEngine(sendInterval, keyInfo);
    ASSERT_TRUE(engine != nullptr);

    EXPECT_EQ(engine->GetEngineName(), "Xbft Engine");
    EXPECT_EQ(engine->GetEngineVersion(), "1.0.0");
    EXPECT_EQ(engine->IsLeader(), false);
    EXPECT_EQ(engine->IsViewActive(), true);
}

TEST_F(XbftEngineTest, TestViewChangeWithHighQc) {
    Tool::FetchMsgAndClear();
    Tool::FetchCommitedAndClear();

    const int64_t sendInterval = 10000000;
    const size_t nodeCount = 4;
    // 构建4共识节点引擎
    std::vector<std::shared_ptr<xbft::XbftEngine>> vecEngine;
    for (size_t i = 0; i < nodeCount; i++) {
        std::shared_ptr<Key> keyInfo = Tool::CreateKeyInfo(std::to_string(i));
        ASSERT_TRUE(keyInfo != nullptr);

        auto engine = XbftEngineTest::CreateXbftEngine(sendInterval, keyInfo);
        ASSERT_TRUE(engine != nullptr);
        vecEngine.push_back(engine);
    }

    auto p_consValue = std::make_shared<ConsensusValue>();
    p_consValue->m_seq = 2;
    p_consValue->m_previousProof = "";

    ASSERT_TRUE(vecEngine[0]->Propose(p_consValue));
    auto vectMsg = Tool::FetchMsgAndClear();

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);
    // 从节点接收处理提案数据
    vecEngine[1]->Recv(vectMsg[0].msg, keyTool);
    vecEngine[2]->Recv(vectMsg[0].msg, keyTool);
    vecEngine[3]->Recv(vectMsg[0].msg, keyTool);
    // 主节点接收投票数据
    vectMsg = Tool::FetchMsgAndClear();
    vecEngine[0]->Recv(vectMsg[0].msg, keyTool);
    vecEngine[0]->Recv(vectMsg[1].msg, keyTool);
    vectMsg = Tool::FetchMsgAndClear();  // clear the decide message
    auto valueCommited = Tool::FetchCommitedAndClear();
    ASSERT_EQ(valueCommited.size(), (size_t)1);

    // new view
    for (size_t i = 0; i < nodeCount - 1; i++) {
        vecEngine[i]->Rotate();
        vectMsg = Tool::FetchMsgAndClear();
        for (size_t m = 0; m < nodeCount; m++) {
            if (m != i) {
                vecEngine[m]->Recv(vectMsg[0].msg, keyTool);
            }
        }
    }

    // The seq:2 must be pre commited
    valueCommited = Tool::FetchCommitedAndClear();
    ASSERT_EQ(valueCommited.size(), (size_t)3);

    for (size_t i = 0; i < nodeCount; i++) {
        ASSERT_EQ(vecEngine[i]->GetViewNumber(), 1);
    }
}

TEST_F(XbftEngineTest, TestViewChangeWithHighQcAndLastTc) {
    Tool::FetchMsgAndClear();
    Tool::FetchCommitedAndClear();

    const int64_t sendInterval = 10000000;
    const size_t nodeCount = 4;
    // 构建4共识节点引擎
    std::vector<std::shared_ptr<xbft::XbftEngine>> vecEngine;
    for (size_t i = 0; i < nodeCount; i++) {
        std::shared_ptr<Key> keyInfo = Tool::CreateKeyInfo(std::to_string(i));
        ASSERT_TRUE(keyInfo != nullptr);

        auto engine = XbftEngineTest::CreateXbftEngine(sendInterval, keyInfo);
        ASSERT_TRUE(engine != nullptr);
        vecEngine.push_back(engine);
    }

    auto p_consValue = std::make_shared<ConsensusValue>();
    p_consValue->m_seq = 2;
    p_consValue->m_previousProof = "";

    ASSERT_TRUE(vecEngine[0]->Propose(p_consValue));
    std::vector<XbftEngineMsg> vectMsg = Tool::FetchMsgAndClear();

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);
    // 从节点接收处理提案数据
    vecEngine[1]->Recv(vectMsg[0].msg, keyTool);
    vecEngine[2]->Recv(vectMsg[0].msg, keyTool);
    vecEngine[3]->Recv(vectMsg[0].msg, keyTool);

    vectMsg = Tool::FetchMsgAndClear();
    vecEngine[0]->Recv(vectMsg[0].msg, keyTool);
    vecEngine[0]->Recv(vectMsg[1].msg, keyTool);

    vectMsg = Tool::FetchMsgAndClear();  // clear the decide message
    std::vector<XbftEngineValueCommited> valueCommited = Tool::FetchCommitedAndClear();
    ASSERT_EQ(valueCommited.size(), (size_t)1);

    // new view
    for (size_t i = 0; i < nodeCount - 1; i++) {
        vecEngine[i]->Rotate();
        vectMsg = Tool::FetchMsgAndClear();
        for (size_t m = 0; m < nodeCount; m++) {
            if (m != i && m != 3) {
                vecEngine[m]->Recv(vectMsg[0].msg, keyTool);
            }
        }
    }

    // node 0,1,2 view number must be 1, node 3 view must be 0
    //    0  1  2
    // 0     x  x
    // 1  x     x
    // 2  x  x
    // 3
    for (size_t i = 0; i < nodeCount - 1; i++) {
        ASSERT_EQ(vecEngine[i]->GetViewNumber(), 1);
    }
    ASSERT_EQ(vecEngine[3]->GetViewNumber(), 0);

    // new view
    for (size_t i = 0; i < nodeCount - 1; i++) {
        vecEngine[i]->Rotate();
        vectMsg = Tool::FetchMsgAndClear();
        for (size_t m = 0; m < nodeCount; m++) {
            if (m != i) {
                vecEngine[m]->Recv(vectMsg[0].msg, keyTool);
            }
        }
    }

    // node 0,1,2 view number must be 2
    //    0  1  2
    // 0     x  x
    // 1  x     x
    // 2  x  x
    // 3  x  x  x
    for (size_t i = 0; i < nodeCount; i++) {
        ASSERT_EQ(vecEngine[i]->GetViewNumber(), 2);
    }
}

TEST_F(XbftEngineTest, TestCreateNormal) {
    Tool::FetchMsgAndClear();
    Tool::FetchCommitedAndClear();

    const int64_t sendInterval = 10000000;
    const size_t nodeCount = 4;
    // 构建4共识节点引擎
    std::vector<std::shared_ptr<xbft::XbftEngine>> vecEngine;
    for (size_t i = 0; i < nodeCount; i++) {
        std::shared_ptr<Key> keyInfo = Tool::CreateKeyInfo(std::to_string(i));
        ASSERT_TRUE(keyInfo != nullptr);

        auto engine = XbftEngineTest::CreateXbftEngine(sendInterval, keyInfo);
        ASSERT_TRUE(engine != nullptr);
        vecEngine.push_back(engine);
    }

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    std::string preProof = "";
    for (int i = 0; i < 10; i++) {
        std::shared_ptr<ConsensusValue> p_consValue = std::make_shared<ConsensusValue>();
        p_consValue->m_seq = i + 2;
        p_consValue->m_previousProof = preProof;

        ASSERT_TRUE(vecEngine[0]->Propose(p_consValue));
        ASSERT_TRUE(vecEngine[0]->IsLeader());
        // propose msg
        std::vector<XbftEngineMsg> vectMsg = Tool::FetchMsgAndClear();
        ASSERT_EQ(vectMsg.size(), (size_t)1);
        ASSERT_EQ(vectMsg[0].m_toAddrs.size(), (size_t)3);
        // onPropose
        vecEngine[1]->Recv(vectMsg[0].msg, keyTool);
        vecEngine[2]->Recv(vectMsg[0].msg, keyTool);
        vecEngine[3]->Recv(vectMsg[0].msg, keyTool);

        std::vector<XbftEngineValueCommited> valueCommited;
        if (i > 0) {
            valueCommited = Tool::FetchCommitedAndClear();
            ASSERT_EQ(valueCommited.size(), (size_t)3);
        }
        // onVote
        vectMsg = Tool::FetchMsgAndClear();
        ASSERT_EQ(vectMsg.size(), (size_t)3);
        vecEngine[0]->Recv(vectMsg[0].msg, keyTool);
        vecEngine[0]->Recv(vectMsg[1].msg, keyTool);
        vectMsg = Tool::FetchMsgAndClear();  // clear the decide message

        valueCommited = Tool::FetchCommitedAndClear();
        ASSERT_EQ(valueCommited.size(), (size_t)1);
        preProof = valueCommited[0].proof;
    }
}