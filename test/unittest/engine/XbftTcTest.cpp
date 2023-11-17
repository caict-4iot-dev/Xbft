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
#include "XbftTc.h"
#include "XbftValidatorSetTest.h"

bool verifyTrue(const std::string &cr_signData, const std::string &cr_signature, const std::string &cr_publicKey) {
    return true;
}

bool verifyFalse(const std::string &cr_signData, const std::string &cr_signature, const std::string &cr_publicKey) {
    return false;
}

std::string pubKeyToAddress(const std::string &cr_publicKey) {
    if (cr_publicKey == "pubkey 1")
        return "test addr 1";
    if (cr_publicKey == "pubkey 2")
        return "test addr 2";
    if (cr_publicKey == "pubkey 3")
        return "test addr 3";
    if (cr_publicKey == "pubkey 4")
        return "test addr 4";
    return "unknow address";
}

TEST_F(XbftTcTest, IsValidTrue) {
    auto keyTool = std::make_shared<xbft::KeyToolInterface>();
    ASSERT_TRUE(keyTool != nullptr);

    keyTool->Verify = verifyTrue;
    keyTool->PublicKeyToAddr = pubKeyToAddress;

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
    sig1.set_public_key("pubkey 1");
    *tc.add_signature() = sig1;

    protocol::Signature sig2;
    sig2.set_public_key("pubkey 2");
    *tc.add_signature() = sig2;

    protocol::Signature sig3;
    sig3.set_public_key("pubkey 3");
    *tc.add_signature() = sig3;

    protocol::Signature sig4;
    sig4.set_public_key("pubkey 4");
    *tc.add_signature() = sig4;

    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    protocol::ValidatorSet validatorsSet;
    validatorsSet.add_validators()->set_address("test addr 1");
    validatorsSet.add_validators()->set_address("test addr 2");
    validatorsSet.add_validators()->set_address("test addr 3");
    validatorsSet.add_validators()->set_address("test addr 4");
    validator.Set(validatorsSet);

    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), true);
}

TEST_F(XbftTcTest, IsValidVerifyFalse) {
    auto keyTool = std::make_shared<xbft::KeyToolInterface>();
    ASSERT_TRUE(keyTool != nullptr);

    keyTool->Verify = verifyFalse;
    keyTool->PublicKeyToAddr = pubKeyToAddress;

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
    sig1.set_public_key("pubkey 1");
    *tc.add_signature() = sig1;

    protocol::Signature sig2;
    sig2.set_public_key("pubkey 2");
    *tc.add_signature() = sig2;

    protocol::Signature sig3;
    sig3.set_public_key("pubkey 3");
    *tc.add_signature() = sig3;

    protocol::Signature sig4;
    sig4.set_public_key("pubkey 4");
    *tc.add_signature() = sig4;

    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    protocol::ValidatorSet validatorsSet;
    validatorsSet.add_validators()->set_address("test addr 1");
    validatorsSet.add_validators()->set_address("test addr 2");
    validatorsSet.add_validators()->set_address("test addr 3");
    validatorsSet.add_validators()->set_address("test addr 4");
    validator.Set(validatorsSet);

    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), false);
}

TEST_F(XbftTcTest, IsValidHighQcNotEqualSigFalse) {
    auto keyTool = std::make_shared<xbft::KeyToolInterface>();
    ASSERT_TRUE(keyTool != nullptr);

    keyTool->Verify = verifyTrue;
    keyTool->PublicKeyToAddr = pubKeyToAddress;

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
    sig1.set_public_key("pubkey 1");
    *tc.add_signature() = sig1;

    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    protocol::ValidatorSet validatorsSet;
    validatorsSet.add_validators()->set_address("test addr 1");
    validatorsSet.add_validators()->set_address("test addr 2");
    validatorsSet.add_validators()->set_address("test addr 3");
    validatorsSet.add_validators()->set_address("test addr 4");
    validator.Set(validatorsSet);

    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), false);
}

TEST_F(XbftTcTest, IsValidCanNotFindValidatorFalse) {
    auto keyTool = std::make_shared<xbft::KeyToolInterface>();
    ASSERT_TRUE(keyTool != nullptr);

    keyTool->Verify = verifyTrue;
    keyTool->PublicKeyToAddr = pubKeyToAddress;

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
    sig1.set_public_key("pubkey 1");
    *tc.add_signature() = sig1;

    protocol::Signature sig2;
    sig2.set_public_key("pubkey 2");
    *tc.add_signature() = sig2;

    protocol::Signature sig3;
    sig3.set_public_key("pubkey 6");
    *tc.add_signature() = sig3;

    protocol::Signature sig4;
    sig4.set_public_key("pubkey 7");
    *tc.add_signature() = sig4;

    auto xtc = std::make_shared<xbft::XbftTc>(tc, keyTool);
    ASSERT_TRUE(xtc != nullptr);

    xbft::XbftValidatorSet validator;
    protocol::ValidatorSet validatorsSet;
    validatorsSet.add_validators()->set_address("test addr 1");
    validatorsSet.add_validators()->set_address("test addr 2");
    validatorsSet.add_validators()->set_address("test addr 3");
    validatorsSet.add_validators()->set_address("test addr 4");
    validator.Set(validatorsSet);

    EXPECT_EQ(xtc->IsValid(validator, validator.GetQuorumSize()), false);
}
