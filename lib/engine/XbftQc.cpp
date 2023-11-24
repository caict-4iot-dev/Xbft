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
 * @file: XbftQc.cpp
 */

#include "XbftQc.h"
#include "Logger.h"
#include "Sha256.h"
#include "Strings.h"

namespace xbft {
XbftQc::XbftQc(int64_t viewNumber, int64_t sequence, const std::string &cr_nodeValueHash) {
    m_qc.mutable_qc_content()->set_view_number(viewNumber);
    m_qc.mutable_qc_content()->set_sequence(sequence);
    m_qc.mutable_qc_content()->set_node_value_hash(cr_nodeValueHash);
    m_contentHash = utils::Sha256::Crypto(m_qc.qc_content().SerializeAsString());
}

XbftQc::XbftQc(const protocol::XbftQc &cr_protoQc) {
    m_qc = cr_protoQc;
}

XbftQc::~XbftQc() = default;

bool XbftQc::Add(int64_t replicaId, const protocol::XbftQcValue &cr_qcValue, const protocol::Signature &cr_sig) {
    if (!m_qc.has_qc_content()) {
        *m_qc.mutable_qc_content() = cr_qcValue;
        *m_qc.add_signatures() = cr_sig;
        m_contentHash = utils::Sha256::Crypto(m_qc.qc_content().SerializeAsString());
        return true;
    }

    do {
        if (cr_qcValue.SerializeAsString() != m_qc.qc_content().SerializeAsString()) {
            LOG_ERROR("Node's hash(%s) not equal with qc's node hash(%s)",
                utils::String::Bin4ToHexString(cr_qcValue.node_value_hash()).c_str(),
                utils::String::Bin4ToHexString(m_qc.qc_content().node_value_hash()).c_str());
            break;
        }

        bool exist = false;
        for (int i = 0; i < m_qc.signatures_size(); i++) {
            const protocol::Signature &item = m_qc.signatures(i);
            if (cr_sig.public_key() == item.public_key()) {
                exist = true;
                LOG_ERROR(
                    "Node' hash(%s) exist in qc", utils::String::Bin4ToHexString(cr_qcValue.node_value_hash()).c_str());
                break;
            }
        }

        if (exist) {
            break;
        }

        *m_qc.add_signatures() = cr_sig;
        return true;
    } while (false);
    return false;
}

const std::string &XbftQc::GetContentHash() const {
    return m_contentHash;
}

size_t XbftQc::GetSignatureSize() const {
    return (size_t)m_qc.signatures_size();
}

std::string XbftQc::GetNodeHash() const {
    return m_qc.qc_content().node_hash();
}

const protocol::XbftQc &XbftQc::GetProtoQc() const {
    return m_qc;
}

std::string XbftQc::GetProtoQcString() const {
    return m_qc.SerializeAsString();
}
}  // namespace xbft
