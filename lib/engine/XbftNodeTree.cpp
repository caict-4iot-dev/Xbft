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
 * @file: XbftNodeTree.cpp
 */

#include "XbftNodeTree.h"
#include "Logger.h"
#include "Strings.h"

namespace xbft {
XbftNodeTree::XbftNodeTree() = default;

XbftNodeTree::~XbftNodeTree() = default;

bool XbftNodeTree::Initialize(XbftNodePointer p_rootNode) {
    m_nodes.clear();
    m_nodes.insert(std::make_pair(p_rootNode->GetHash(), p_rootNode));
    m_leafNodes.clear();
    m_leafNodes.push_back(p_rootNode);
    return true;
}

XbftNodePointer XbftNodeTree::GetNode(const std::string &cr_hash) {
    auto iter = m_nodes.find(cr_hash);
    if (iter == m_nodes.end()) {
        return nullptr;
    }

    return iter->second;
}

XbftNodePointer XbftNodeTree::GetParentNode(const std::string &cr_hash) {
    auto iter = m_nodes.find(cr_hash);
    do {
        if (iter == m_nodes.end()) {
            // cann't find the node
            LOG_ERROR("Cann't find the node, hash(%s)", cr_hash.c_str());
            break;
        }
        auto iter_parent = m_nodes.find(iter->second->GetParentHash());
        if (iter_parent == m_nodes.end()) {
            LOG_ERROR("Cann't find the parent node, hash(%s)",
                utils::String::Bin4ToHexString(iter->second->GetParentHash()).c_str());
            // cann't find the parent node
            break;
        }

        return iter_parent->second;

    } while (false);

    return nullptr;
}

bool XbftNodeTree::AppendNode(const XbftNodePointer &cp_node) {
    auto iter = m_nodes.find(cp_node->GetParentHash());
    do {
        if (iter == m_nodes.end()) {
            // cann't find the node
            LOG_WARN("Cann't find the parent node, hash(%s)",
                utils::String::Bin4ToHexString(cp_node->GetParentHash()).c_str());
        } else {
            if (iter->second->GetSequence() != cp_node->GetSequence() - 1) {
                LOG_ERROR("Parent node's sequence(%ld + 1) not equal with node's sequence(%ld)",
                    iter->second->GetSequence(), cp_node->GetSequence());
                break;
            }
        }
        // LOG_INFO("Add node, hash(%s), parent hash(%s)", utils::String::Bin4ToHexString(node->GetHash()).c_str(),
        //    utils::String::Bin4ToHexString(node->GetParentHash()).c_str());
        m_nodes[cp_node->GetHash()] = cp_node;
        for (auto iterDel = m_leafNodes.begin(); iterDel != m_leafNodes.end(); iterDel++) {
            if ((*iterDel)->GetHash() == cp_node->GetParentHash()) {
                m_leafNodes.erase(iterDel);
                break;
            }
        }
        m_leafNodes.push_back(cp_node);

        return true;
    } while (false);

    return false;
}

size_t XbftNodeTree::GetSize() const {
    return m_nodes.size();
}

int32_t XbftNodeTree::ReleaseFrom(const std::string &cr_hash) {
    std::string currentHash = cr_hash;
    int32_t releasedCounter = 0;
    do {
        auto iter = m_nodes.find(currentHash);
        if (iter != m_nodes.end()) {
            currentHash = iter->second->GetParentHash();
            m_nodes.erase(iter);
            releasedCounter++;
            continue;
        }
        break;
    } while (true);

    return releasedCounter;
}

std::string XbftNodeTree::GetHighestLeafHash() const {
    int64_t highSeq = -1;
    std::string highHash;
    std::for_each(m_leafNodes.begin(), m_leafNodes.end(), [&highSeq, &highHash](XbftNodePointer ref) {
        if (ref->GetSequence() > highSeq) {
            highSeq = ref->GetSequence();
            highHash = ref->GetHash();
        }
    });

    return highHash;
}
}  // namespace xbft
