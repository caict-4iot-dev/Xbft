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
 * @date: 2023-11-20 17:03:19
 * @file: XbftConsensusTest.h
 */

#ifndef __XBFTCONSENSUS_TEST_H__
#define __XBFTCONSENSUS_TEST_H__

#include "consensus.pb.h"
#include <gtest/gtest.h>

namespace xbft {
using XbftEnvPointer = std::shared_ptr<protocol::XbftEnv>;
}  // namespace xbft

class XbftConsensusTest : public testing::Test {
public:
    static xbft::XbftEnvPointer CreateProposeMsg(int64_t replicaId, int64_t viewNumber);
    static xbft::XbftEnvPointer CreateVoteMsg(int64_t replicaId, int64_t viewNumber, int64_t sequence);
};
#endif
