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
 * @date: 2023-11-16 10:55:55
 * @file: Sha256Test.cpp
 */


#include "Sha256Test.h"
#include "Sha256.h"
#include "Strings.h"

TEST_F(Sha256Test, HashString) {
    std::string data = "aaa";
    std::string hash_data1 = utils::Sha256::Crypto(data);
    ASSERT_EQ(
        utils::String::BinToHexString(hash_data1), "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");

    std::string hash_data2;
    utils::Sha256::Crypto(data, hash_data2);
    ASSERT_EQ(
        utils::String::BinToHexString(hash_data2), "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");
}
