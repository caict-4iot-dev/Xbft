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
 * @date: 2023-11-17 11:29:29
 * @file: XbftTcTest.cpp
 */

#include "XbftTcTest.h"
#include "ConsEngine.h"
#include "Tool.h"
#include "XbftTc.h"
#include "XbftValidatorSetTest.h"


TEST_F(XbftTcTest, IsValid) {
    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    // message XbftTc {
    //     int64 view_number = 1;
    //     repeated int64 high_qc_view_number = 2;
    //     repeated Signature signature = 3;
    // }
    protocol::XbftTc tc;
    tc.set_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    protocol::Signature sig0;
    sig0.set_public_key("pub-0");
    *tc.add_signature() = sig0;

    protocol::Signature sig1;
    sig1.set_public_key("pub-1");
    *tc.add_signature() = sig1;

    protocol::Signature sig2;
    sig2.set_public_key("pub-2");
    *tc.add_signature() = sig2;

    protocol::Signature sig3;
    sig3.set_public_key("pub-3");
    *tc.add_signature() = sig3;

    // tc verify true
    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    validator.Update("address-0", 0);
    validator.Update("address-1", 1);
    validator.Update("address-2", 2);
    validator.Update("address-3", 3);
    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), true);
    // tc verify false
    keyTool->m_verify = toolMock::verifyFalse;
    auto xtc2 = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator2;
    validator2.Update("address-4", 4);
    validator2.Update("address-5", 5);
    validator2.Update("address-6", 6);
    validator2.Update("address-7", 7);

    EXPECT_EQ(xtc->IsValid(validator2, validator.GetQuorumSize()), false);
}

TEST_F(XbftTcTest, IsValidHighQcNotEqualSigFalse) {
    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    // message XbftTc {
    //     int64 view_number = 1;
    //     repeated int64 high_qc_view_number = 2;
    //     repeated Signature signature = 3;
    // }
    protocol::XbftTc tc;
    tc.set_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    protocol::Signature sig1;
    sig1.set_public_key("pub-1");
    *tc.add_signature() = sig1;

    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    validator.Update("address-0", 0);
    validator.Update("address-1", 1);
    validator.Update("address-2", 2);
    validator.Update("address-3", 3);

    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), false);
}

TEST_F(XbftTcTest, IsValidVerifyFalse) {
    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);
    keyTool->m_verify = toolMock::verifyFalse;

    // message XbftTc {
    //     int64 view_number = 1;
    //     repeated int64 high_qc_view_number = 2;
    //     repeated Signature signature = 3;
    // }
    protocol::XbftTc tc;
    tc.set_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    tc.add_high_qc_view_number(10);
    protocol::Signature sig0;
    sig0.set_public_key("pub-0");
    *tc.add_signature() = sig0;

    protocol::Signature sig1;
    sig1.set_public_key("pub-1");
    *tc.add_signature() = sig1;

    protocol::Signature sig2;
    sig2.set_public_key("pub-2");
    *tc.add_signature() = sig2;

    protocol::Signature sig3;
    sig3.set_public_key("pub-3");
    *tc.add_signature() = sig3;

    // tc verify true
    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    validator.Update("address-0", 0);
    validator.Update("address-1", 1);
    validator.Update("address-2", 2);
    validator.Update("address-3", 3);
    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), false);
}
