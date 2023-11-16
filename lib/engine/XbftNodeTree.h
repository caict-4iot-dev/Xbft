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
 * @date: 2023-11-15 15:08:21
 * @file: XbftNodeTree.h
 */

#ifndef __XBFTNODETREE_H__
#define __XBFTNODETREE_H__

#include "XbftNode.h"
#include <list>
#include <unordered_map>

namespace xbft {
class XbftNodeTree {
public:
    XbftNodeTree();
    ~XbftNodeTree();

    bool Initialize(XbftNodePointer p_rootNode);
    XbftNodePointer GetNode(const std::string &cr_hash);        // Get node pointer by hash
    XbftNodePointer GetParentNode(const std::string &cr_hash);  // Get parent Node by hash
    bool AppendNode(const XbftNodePointer &cp_node);
    size_t GetSize() const;
    int32_t ReleaseFrom(const std::string &cr_hash);  // There's some bug, need to be upgrade
    std::string GetHighestLeafHash() const;

private:
    std::unordered_map<std::string, XbftNodePointer> m_nodes;
    XbftNodePointer mp_rootNode;
    std::list<XbftNodePointer> m_leafNodes;
};
}  // namespace xbft

#endif