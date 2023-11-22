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
 * @date: 2023-11-17 15:07:10
 * @file: XbftNodeTreeTest.cpp
 */


#include "XbftNodeTreeTest.h"
#include "ConsEngine.h"
#include "Strings.h"
#include "Tool.h"
#include "XbftNode.h"
#include "XbftNodeTree.h"


std::shared_ptr<xbft::XbftNode> XbftNodeTreeTest::CreateLeafNode(int64_t sequence, int64_t viewNumber,
    const std::string &cr_rootHash, std::shared_ptr<xbft::KeyToolInterface> p_keyTool) {
    protocol::XbftNode pnode;
    pnode.set_parent_node_hash(cr_rootHash);
    pnode.set_sequence(sequence);
    pnode.set_view_number(viewNumber);
    return std::make_shared<xbft::XbftNode>(pnode, p_keyTool);
}

TEST_F(XbftNodeTreeTest, test) {
    xbft::XbftNodeTree nodeTree;

    auto keyTool = Tool::CreateKeyTool();
    ASSERT_TRUE(keyTool != nullptr);

    auto node = std::make_shared<xbft::XbftNode>(1, 1, keyTool);
    ASSERT_TRUE(node != nullptr);

    // 同一个节点可添加多次，覆盖
    EXPECT_EQ(nodeTree.AppendNode(node), true);
    EXPECT_EQ(nodeTree.GetSize(), (size_t)1);
    EXPECT_EQ(nodeTree.AppendNode(node), true);
    EXPECT_EQ(nodeTree.GetSize(), (size_t)1);

    auto rootHash = node->GetHash();

    // 构造多颗子树

    /*     x->x->x
        x
           x->x
    */
    auto nodeL1 = CreateLeafNode(2, 2, rootHash, keyTool);
    EXPECT_EQ(nodeTree.AppendNode(nodeL1), true);
    auto nodeL2 = CreateLeafNode(3, 3, nodeL1->GetHash(), keyTool);
    EXPECT_EQ(nodeTree.AppendNode(nodeL2), true);
    auto nodeL3 = CreateLeafNode(4, 4, nodeL2->GetHash(), keyTool);
    EXPECT_EQ(nodeTree.AppendNode(nodeL3), true);

    auto nodeR1 = CreateLeafNode(2, 5, rootHash, keyTool);
    EXPECT_EQ(nodeTree.AppendNode(nodeR1), true);
    auto nodeR2 = CreateLeafNode(3, 6, nodeR1->GetHash(), keyTool);
    EXPECT_EQ(nodeTree.AppendNode(nodeR2), true);

    EXPECT_EQ(nodeTree.GetSize(), (size_t)6);

    EXPECT_EQ(utils::String::Bin4ToHexString(nodeTree.GetHighestLeafHash()),
        utils::String::Bin4ToHexString(nodeL3->GetHash()));

    EXPECT_EQ(nodeTree.GetParentNode(nodeL1->GetHash())->GetHash(), rootHash);
    EXPECT_EQ(nodeTree.GetParentNode(nodeR1->GetHash())->GetHash(), rootHash);

    EXPECT_EQ(nodeTree.ReleaseFrom(nodeR1->GetHash()), 2);

    EXPECT_EQ(nodeTree.GetSize(), (size_t)4);
}
