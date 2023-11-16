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
 * @file: XbftNode.cpp
 */

#include "XbftNode.h"
#include "ConsEngine.h"
#include "Logger.h"
#include "Sha256.h"
#include "Strings.h"
#include "Timestamp.h"

namespace xbft {
// this contruct function is for genesis node
XbftNode::XbftNode(int64_t viewNumber, int64_t sequence, std::shared_ptr<KeyToolInterface> p_keyTool) {
    m_viewNumber = viewNumber;
    m_sequence = sequence;

    m_decision = true;
    m_preCommit = true;
    mp_qcRef = nullptr;
    mp_consValue = nullptr;
    m_parentHash = "";

    m_node.set_node_value("");
    m_node.set_parent_node_hash("");
    m_node.set_view_number(m_viewNumber);
    m_node.set_sequence(m_sequence);
    m_hash = crypto::Sha256::Crypto(m_node.SerializeAsString());

    m_startTime = utils::Timestamp::HighResolution();
    m_endTime = 0;
    m_lastProposeTime = 0;
    m_proposeRound = 0;
    mp_keyTool = p_keyTool;
}
XbftNode::XbftNode(int64_t viewNumber, int64_t sequence, const protocol::XbftQc &cr_protoQc,
    const std::string &cr_parentHash, std::shared_ptr<ConsData> p_consValue,
    std::shared_ptr<KeyToolInterface> p_keyTool) {
    m_viewNumber = viewNumber;
    m_sequence = sequence;
    m_parentHash = cr_parentHash;
    m_decision = false;
    m_preCommit = false;
    mp_qcRef = nullptr;
    mp_consValue = std::move(p_consValue);

    mp_qc = std::make_shared<XbftQc>(cr_protoQc);

    m_node.set_node_value(mp_consValue->GetStringValue());
    m_node.set_parent_node_hash(m_parentHash);
    m_node.set_view_number(m_viewNumber);
    m_node.set_sequence(m_sequence);
    *m_node.mutable_previous_qc() = cr_protoQc;
    m_hash = crypto::Sha256::Crypto(m_node.SerializeAsString());

    m_startTime = utils::Timestamp::HighResolution();
    m_endTime = 0;
    m_lastProposeTime = 0;
    m_proposeRound = 0;
    mp_keyTool = p_keyTool;
}

XbftNode::XbftNode(const protocol::XbftNode &cr_node, std::shared_ptr<KeyToolInterface> p_keyTool) {
    m_viewNumber = cr_node.view_number();
    m_sequence = cr_node.sequence();
    m_parentHash = cr_node.parent_node_hash();
    m_decision = false;
    m_preCommit = false;
    mp_qcRef = nullptr;

    mp_qc = std::make_shared<XbftQc>(cr_node.previous_qc());
    m_node = cr_node;
    m_hash = crypto::Sha256::Crypto(m_node.SerializeAsString());
    m_startTime = utils::Timestamp::HighResolution();
    m_endTime = 0;
    m_lastProposeTime = 0;
    m_proposeRound = 0;
    mp_keyTool = p_keyTool;
}

XbftNode::~XbftNode() = default;

const std::string &XbftNode::GetHash() const {
    return m_hash;
}

const std::string &XbftNode::GetParentHash() const {
    return m_parentHash;
}

const int64_t &XbftNode::GetViewNumber() const {
    return m_viewNumber;
}

const int64_t &XbftNode::GetSequence() const {
    return m_sequence;
}

void XbftNode::SetProposeMsg(XbftEnvPointer p_msg) {
    mp_propseMsg = std::move(p_msg);
}

const protocol::XbftNode &XbftNode::GetProtoNode() const {
    return m_node;
}

void XbftNode::UpdateQcRef(XbftNodePointer p_node) {
    mp_qcRef = std::move(p_node);
}

const XbftQcPointer &XbftNode::GetQc() const {
    return mp_qc;
}

const XbftNodePointer &XbftNode::GetQcRef() const {
    return mp_qcRef;
}

bool XbftNode::GetDecision() const {
    return m_decision;
}

bool XbftNode::GetPreCommit() const {
    return m_preCommit;
}

void XbftNode::SetDecision(bool decision) {
    m_decision = decision;
}

void XbftNode::SetPreCommit(bool commited) {
    m_preCommit = commited;
}

std::string XbftNode::GetDesc() const {
    return utils::String::Format("view:%ld,seq:%ld,hash:%s,phash(%s), qhash(%s)", m_viewNumber, m_sequence,
        utils::String::Bin4ToHexString(GetHash()).c_str(), utils::String::Bin4ToHexString(GetParentHash()).c_str(),
        utils::String::Bin4ToHexString(m_node.previous_qc().qc_content().node_hash()).c_str());
}

std::shared_ptr<ConsData> XbftNode::GetConsensusValue() {
    return mp_consValue;
}

size_t XbftNode::GetVotedSize() const {
    return m_voted.size();
}

bool XbftNode::AddVote(int64_t replicaId) {
    return m_voted.insert(replicaId).second;
}

XbftQcPointer &XbftNode::GetSelfQc() {
    return mq_selfQc;
}

void XbftNode::InitSelfQc() {
    protocol::XbftQc qcEmpty;
    mq_selfQc = std::make_shared<XbftQc>(qcEmpty);
}

void XbftNode::SetValidators(const XbftValidatorSet &cr_validators) {
    m_validators = cr_validators;
}

bool XbftNode::NeedSendProposeAgain(int64_t current_time) const {
    if (m_lastProposeTime > 0 && current_time - m_lastProposeTime > g_xbft_instance_timeout_ / 2 && m_endTime == 0) {
        return true;
    }

    return false;
}

void XbftNode::SetLastProposeTime(int64_t current_time, int64_t round) {
    m_lastProposeTime = current_time;
    m_proposeRound = round;
}

int64_t XbftNode::GetProposeRound() const {
    return m_proposeRound;
}

void XbftNode::SetEndTime(int64_t currentTime) {
    m_endTime = currentTime;
}

void XbftNode::SetHash(const std::string &cr_hash) {
    m_hash = cr_hash;
}

bool XbftNode::CheckQc(const XbftQcPointer &cr_selfQc) const {
    if (m_validators.Empty()) {
        return true;
    }

    const protocol::XbftQc &protoQc = cr_selfQc->GetProtoQc();
    const protocol::XbftQcValue &qcValue = protoQc.qc_content();
    std::string protoValueStr = qcValue.SerializeAsString();
    std::set<std::string> validValidators;
    for (int i = 0; i < protoQc.signatures_size(); i++) {
        const protocol::Signature &sig = protoQc.signatures(i);

        std::string addr = mp_keyTool->PublicKeyToAddr(sig.public_key());
        if (-1 == m_validators.QueryId(addr)) {
            LOG_WARN("Cann't find the tc'validator from list");
            continue;
        }
        if (!mp_keyTool->Verify(protoValueStr, sig.sign_data(), sig.public_key())) {
            LOG_WARN("Failed to verify the tc's signature");
            continue;
        }
        validValidators.insert(addr);
    }

    return validValidators.size() >= m_validators.GetQuorumSize();
}
}  // namespace xbft
