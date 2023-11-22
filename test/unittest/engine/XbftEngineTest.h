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
 * @date: 2023-11-20 18:19:44
 * @file: XbftEngineTest.h
 */

#ifndef __XBFTENGINE_TEST_H__
#define __XBFTENGINE_TEST_H__

#include <gtest/gtest.h>
#include "ConsEngine.h"
#include "Tool.h"

namespace xbft {
class XbftEngine;
}

class Key;
class XbftEngineTest : public testing::Test {
public:
    static std::shared_ptr<xbft::XbftEngine> CreateXbftEngine(int64_t interVal, std::shared_ptr<Key> p_key);
};

#endif
