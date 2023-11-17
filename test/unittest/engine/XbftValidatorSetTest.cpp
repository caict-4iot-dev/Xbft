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
 * @date: 2023-11-17 10:58:20
 * @file: XbftValidatorSetTest.cpp
 */

#include "XbftValidatorSetTest.h"
#include "XbftValidatorSet.h"
#include "consensus.pb.h"


TEST_F(XbftValidatorSetTest, baseFun) {
    auto validator = std::make_shared<xbft::XbftValidatorSet>();
    ASSERT_TRUE(validator != nullptr);
    EXPECT_EQ(validator->Empty(), true);
    validator->Insert("test addr 1", 1);
    validator->Insert("test addr 2", 2);
    validator->Insert("test addr 3", 3);
    validator->Insert("test addr 4", 4);
    EXPECT_EQ(validator->Size(), (size_t)4);
    EXPECT_EQ(validator->Empty(), false);

    validator->Update("test addr 1", 5);
    EXPECT_EQ(validator->QueryAddr(5), "test addr 1");
    EXPECT_EQ(validator->QueryId("test addr 1"), 5);

    EXPECT_EQ(validator->GetQuorumSize(), (size_t)3);
    EXPECT_EQ(validator->GetValidatorId("test addr 2"), 2);

    std::vector<std::string> vec;
    validator->GetOtherReplicaAddrs(vec, 5);
    EXPECT_EQ(vec.size(), (size_t)3);
}

TEST_F(XbftValidatorSetTest, setAndCompare) {
    auto validator = std::make_shared<xbft::XbftValidatorSet>();
    ASSERT_TRUE(validator != nullptr);

    protocol::ValidatorSet validatorsSet;
    validatorsSet.add_validators()->set_address("test addr 1");
    validatorsSet.add_validators()->set_address("test addr 2");
    validatorsSet.add_validators()->set_address("test addr 3");
    validatorsSet.add_validators()->set_address("test addr 4");
    validatorsSet.add_validators()->set_address("test addr 5");

    validator->Set(validatorsSet);
    EXPECT_EQ(validator->Size(), (size_t)5);
    EXPECT_EQ(validator->GetValidatorId("test addr 5"), 4);

    EXPECT_EQ(validator->Compare(validatorsSet), false);

    validatorsSet.add_validators()->set_address("test addr 6");
    EXPECT_EQ(validator->Compare(validatorsSet), true);
}
