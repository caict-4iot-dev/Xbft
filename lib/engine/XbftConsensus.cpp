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
 * @file: XbftConsensus.cpp
 */

#include "XbftConsensus.h"
#include "Logger.h"
#include "Sha256.h"
#include "Strings.h"
#include "Timestamp.h"
#include "utils/Strings.h"

namespace xbft {
XbftConsensus::XbftConsensus(std::shared_ptr<NetInterface> p_net, std::shared_ptr<ValueDealInterface> p_valueDeal,
    std::shared_ptr<KeyToolInterface> p_keyTool) {
    m_faultNumber = 0;
    m_viewNumber = 0;
    m_lastSequence = 1;
    m_replicaId = -1;
    m_isViewActive = true;  // view-active = false 表示正在进行view-change
    mp_valueDealing = p_valueDeal;
    mp_net = p_net;
    mp_keyTool = p_keyTool;
}

XbftConsensus::~XbftConsensus() = default;

bool XbftConsensus::Initialize(std::shared_ptr<KeyInterface> p_keyInfo,
    const std::vector<std::string> &cr_validatorsVec, int64_t viewNumber, const std::string &cr_lastProof) {
    do {
        LOG_INFO(
            "XbftConsensus::Initialize m_viewNumber:%ld address:%s", m_viewNumber, p_keyInfo->GetAddress().c_str());
        m_viewNumber = viewNumber;
        mp_replicaKey = p_keyInfo;

        protocol::ValidatorSet validators;
        for (auto address : cr_validatorsVec) {
            validators.add_validators()->set_address(address);
            LOG_INFO("XbftConsensus::Initialize Validator:%s", address.c_str());
        }
        // must update validator firstly
        UpdateValidators(validators);
        // then update proof
        return UpdateProof(cr_lastProof);

    } while (false);

    return false;
}

void XbftConsensus::UpdateValidators(const protocol::ValidatorSet &cr_validatorSet) {
    m_validators.Set(cr_validatorSet);
    m_replicaId = m_validators.GetValidatorId(mp_replicaKey->GetAddress());
}

bool XbftConsensus::UpdateValidatorsAndProof(
    const std::vector<std::string> &cr_validators, const std::string &cr_lastProof) {
    if (!UpdateProof(cr_lastProof)) {
        LOG_ERROR("Failed to update proof");
        return false;
    }

    if (!cr_validators.empty()) {
        protocol::ValidatorSet validatorSet;
        for (auto validator : cr_validators) {
            validatorSet.add_validators()->set_address(validator);
        }

        if (m_validators.Compare(validatorSet)) {
            UpdateValidators(validatorSet);

            if (m_validators.Size() < 4) {
                LOG_WARN("Xbft couldn't tolerate fault node when validator size =%ld", m_validators.Size());
            }

            m_faultNumber = (m_validators.Size() - 1) / 3;

            LOG_INFO(
                "When validator size = %ld, pbft can tolerate %ld fault nodes, Current node's replica_id = %ld, so it "
                "%s a "
                "leader",
                m_validators.Size(), m_faultNumber, m_replicaId,
                m_viewNumber % (int64_t)m_validators.Size() == m_replicaId ? "is" : "isnot");
        }
    }

    return true;
}

bool XbftConsensus::UpdateProof(const std::string &cr_lastProof) {
    int64_t newSeq = -1;
    int64_t newViewNumber = -1;
    do {
        if (cr_lastProof.empty()) {
            XbftNodePointer tmp = std::make_shared<XbftNode>(m_viewNumber, m_lastSequence, mp_keyTool);
            m_nodeTree.AppendNode(tmp);
            mp_lock = tmp;
            mp_exec = tmp;
            m_highNodeAndQc = std::make_pair(tmp, nullptr);
            LOG_INFO("Create genesis node(view:%ld, seq:%ld, hash:%s)", m_viewNumber, m_lastSequence,
                utils::String::Bin4ToHexString(tmp->GetHash()).c_str());
            return true;
        }

        protocol::XbftProof xbftProof;
        if (!xbftProof.ParseFromString(cr_lastProof)) {
            LOG_ERROR("Failed to parse proof string");
            break;
        }

        const protocol::XbftNode &node = xbftProof.node();
        const protocol::XbftQc &xbftQc = xbftProof.qc();
        // Compare view number
        if (!xbftQc.has_qc_content()) {
            LOG_ERROR("Qc doesn't have content");
            break;
        }
        if (xbftQc.qc_content().view_number() >= m_viewNumber) {
            newViewNumber = xbftQc.qc_content().view_number();
        }
        if (xbftQc.qc_content().sequence() > m_lastSequence) {
            newSeq = xbftQc.qc_content().sequence();
        }
        LOG_INFO("XbftConsensus::UpdateProof newViewNumber:%ld newSeq:%ld", newViewNumber, newSeq);

        // 如果QC节点的seq超出最新的树节点seq，则创建新节点追加到树中。以新节点为基础继续共识。
        if (newSeq > -1) {
            m_lastSequence = newSeq;
            XbftNodePointer tmp = std::make_shared<XbftNode>(node, mp_keyTool);
            tmp->SetHash(xbftQc.qc_content().node_hash());  // becasue the xbftProof.node.node_value == "", so the hash
                                                            // is not right, we should explitly set it;
            ///===========must set the node validators=================
            tmp->SetValidators(m_validators);
            tmp->SetDecision(true);
            m_nodeTree.AppendNode(tmp);
            LOG_INFO("Setting the last node (sequence:%ld, hash:%s)", m_lastSequence,
                utils::String::Bin4ToHexString(tmp->GetHash()).c_str());

            mp_lock = tmp;
            mp_exec = tmp;
            m_highNodeAndQc = std::make_pair(tmp, std::make_shared<XbftQc>(xbftQc));
        }

        if (newViewNumber > -1) {
            m_isViewActive = true;
            m_viewNumber = newViewNumber;
            mp_valueDealing->m_viewChange();
            LOG_INFO("Setting the last execution view(%ld)", m_viewNumber);
        }

        return true;
    } while (false);

    return false;
}

bool XbftConsensus::Exit() {
    return true;
}

bool XbftConsensus::IsLeader() const {
    if (m_validators.Empty()) {
        LOG_ERROR("Validator set is empty");
        return false;
    }

    return m_viewNumber % (int64_t)m_validators.Size() == m_replicaId;
}

bool XbftConsensus::IsValidator() const {
    return m_replicaId >= 0;
}

int64_t XbftConsensus::GetViewNumber() const {
    return m_viewNumber;
}

int64_t XbftConsensus::GetLeaderId() const {
    return m_viewNumber % m_validators.Size();
}

std::string XbftConsensus::GetLastProof() const {
    return m_lastProof;
}

void XbftConsensus::initSelfQc(const XbftNodePointer &p_node) {
    protocol::XbftQcValue qcValue;
    qcValue.set_node_value_hash(p_node->GetConsensusValue()->GetHash());
    qcValue.set_node_hash(p_node->GetHash());
    qcValue.set_view_number(p_node->GetViewNumber());
    qcValue.set_sequence(p_node->GetSequence());

    protocol::Signature sig;
    sig.set_public_key(mp_replicaKey->GetPublicKey());
    sig.set_sign_data(mp_replicaKey->Sign(qcValue.SerializeAsString()));

    p_node->InitSelfQc();
    p_node->GetSelfQc()->Add(m_replicaId, qcValue, sig);
    p_node->AddVote(m_replicaId);
}

bool XbftConsensus::Propose(std::shared_ptr<ConsData> p_consData) {
    if (!IsLeader()) {
        LOG_ERROR("current node is not leader, forbidden propose consensus data");
        return false;
    }

    if (p_consData->GetSeq() < m_lastSequence) {
        LOG_ERROR(
            "Consensus value sequence(%ld)should larger than last sequence(%ld)", p_consData->GetSeq(), m_lastSequence);
        return false;
    }

    m_lastSequence = p_consData->GetSeq();
    LOG_INFO("Replica(%ld) start to propose view(%ld) seq(%ld)", m_replicaId, m_viewNumber, m_lastSequence);

    protocol::XbftQc preQc;
    const std::string &proof = p_consData->GetPreviousProof();
    std::string prarentNodeHash;
    if (!proof.empty()) {
        protocol::XbftProof protoProof;
        protoProof.ParseFromString(proof);
        preQc = protoProof.qc();
        prarentNodeHash = preQc.qc_content().node_hash();
    } else {
        prarentNodeHash = m_nodeTree.GetHighestLeafHash();
    }

    XbftNodePointer p_node =
        std::make_shared<XbftNode>(m_viewNumber, p_consData->GetSeq(), preQc, prarentNodeHash, p_consData, mp_keyTool);
    p_node->SetValidators(m_validators);

    // Initialize node self qc
    initSelfQc(p_node);

    // update node previous qc
    XbftNodePointer p_qcNode = m_nodeTree.GetNode(preQc.qc_content().node_hash());
    if (p_qcNode == nullptr) {
        LOG_WARN("Cann't find the qc node by hash(%s)",
            utils::String::Bin4ToHexString(preQc.qc_content().node_hash()).c_str());
    }
    p_node->UpdateQcRef(p_qcNode);

    // Append the node
    if (!m_nodeTree.AppendNode(p_node)) {
        LOG_ERROR("Failed to append new node to tree(view:%ld, seq:%ld)", m_viewNumber, p_consData->GetSeq());
        return false;
    }
    XbftMsgPointer env = newPropose(p_node);
    mp_net->m_sendMsg(mp_replicaKey->GetAddress(), GetOtherReplicaAddrs(), env->GetXbftEnv().SerializeAsString());
    p_node->SetLastProposeTime(utils::Timestamp::HighResolution(), 0);

    return true;
}

bool XbftConsensus::SendPropseAgain(const XbftNodePointer &p_node) {
    int64_t currentRound = p_node->GetProposeRound() + 1;
    XbftMsgPointer env = newPropose(p_node, currentRound);
    mp_net->m_sendMsg(mp_replicaKey->GetAddress(), GetOtherReplicaAddrs(), env->GetXbftEnv().SerializeAsString());
    p_node->SetLastProposeTime(utils::Timestamp::HighResolution(), currentRound);
    return true;
}

XbftMsgPointer XbftConsensus::newPropose(const XbftNodePointer &p_node, int64_t round) const {
    XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(round);
    p_xbft->set_type(protocol::XBFT_TYPE_PROPOSE);
    protocol::XbftPropose *p_propose = p_xbft->mutable_propose();
    p_propose->set_replica_id(m_replicaId);
    *p_propose->mutable_node() = p_node->GetProtoNode();
    p_env->mutable_signature()->set_public_key(mp_replicaKey->GetPublicKey());
    p_env->mutable_signature()->set_sign_data(mp_replicaKey->Sign(p_xbft->SerializeAsString()));

    XbftMsgPointer att = std::make_shared<XbftMsg>(*p_env, mp_keyTool);
    return att;
}

XbftMsgPointer XbftConsensus::newDecide(const XbftNodePointer &p_node, int64_t round) const {
    XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(round);
    p_xbft->set_type(protocol::XBFT_TYPE_DECIDE);
    protocol::XbftDecide *p_decide = p_xbft->mutable_decide();
    p_decide->set_replica_id(m_replicaId);
    *p_decide->mutable_qc() = p_node->GetSelfQc()->GetProtoQc();
    p_env->mutable_signature()->set_public_key(mp_replicaKey->GetPublicKey());
    p_env->mutable_signature()->set_sign_data(mp_replicaKey->Sign(p_xbft->SerializeAsString()));

    XbftMsgPointer att = std::make_shared<XbftMsg>(*p_env, mp_keyTool);
    return att;
}

XbftMsgPointer XbftConsensus::newVote(
    const protocol::XbftPropose &propose, const std::string &cr_nodeValueHash, int64_t roundNumber) const {
    XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(roundNumber);
    p_xbft->set_type(protocol::XBFT_TYPE_VOTE);
    protocol::XbftVote *p_vote = p_xbft->mutable_vote();
    p_vote->set_replica_id(m_replicaId);
    protocol::XbftQcValue *qcValue = p_vote->mutable_qc_content();

    std::string node_hash = utils::Sha256::Crypto(propose.node().SerializeAsString());
    qcValue->set_node_hash(node_hash);
    qcValue->set_node_value_hash(cr_nodeValueHash);
    qcValue->set_view_number(propose.node().view_number());
    qcValue->set_sequence(propose.node().sequence());

    p_vote->mutable_signature()->set_public_key(mp_replicaKey->GetPublicKey());
    p_vote->mutable_signature()->set_sign_data(mp_replicaKey->Sign(qcValue->SerializeAsString()));

    p_env->mutable_signature()->set_public_key(mp_replicaKey->GetPublicKey());
    p_env->mutable_signature()->set_sign_data(mp_replicaKey->Sign(p_xbft->SerializeAsString()));
    return std::make_shared<XbftMsg>(*p_env, mp_keyTool);
}

bool XbftConsensus::OnRecv(const XbftMsgPointer &p_xbftMsg) {
    do {
        LOG_INFO("Replica(%ld), Receive (%s) message", m_replicaId, p_xbftMsg->GetDesc().c_str());
        // Check message's format and signature
        if (!p_xbftMsg->CheckMsgItem(m_validators)) {
            LOG_ERROR("Failed to check xbft message");
            break;
        }

        // This view must be active
        if (!m_isViewActive) {
            LOG_ERROR("This view(number:%ld) is not active", m_viewNumber);
            break;
        }

        // The message's view number must be >= local view number
        if (p_xbftMsg->GetViewNumber() < m_viewNumber) {
            LOG_ERROR("The xbft message's view number(%ld) is lower than local view number(%ld)",
                p_xbftMsg->GetViewNumber(), m_viewNumber);
            break;
        }

        bool ret = false;
        if (p_xbftMsg->GetType() == protocol::XBFT_TYPE_PROPOSE) {
            ret = onPropose(p_xbftMsg);
        } else if (p_xbftMsg->GetType() == protocol::XBFT_TYPE_VOTE) {
            ret = onVote(p_xbftMsg);
        } else if (p_xbftMsg->GetType() == protocol::XBFT_TYPE_DECIDE) {
            ret = onDecide(p_xbftMsg);
        }

        return ret;
    } while (false);
    return false;
}

bool XbftConsensus::onVote(const XbftMsgPointer &p_xbftMsg) {
    do {
        // Continue if it has only one node.
        if (!IsLeader()) {
            LOG_WARN("Received VOTE message, but replica would not process");
            return true;
        }

        std::string nodeHash = p_xbftMsg->GetXbftEnv().xbft().vote().qc_content().node_hash();
        XbftNodePointer p_node = m_nodeTree.GetNode(nodeHash);
        if (!p_node) {
            LOG_WARN("Received VOTE message, but cann't find the node from node tree");
            break;
        }

        // ================if receive enough vote, return true, not break==============
        if (p_node->GetVotedSize() >= GetQuorumSize()) {
            LOG_TRACE("Received enough vote(count:%ld), ignore this vote message(from:%s)", p_node->GetVotedSize(),
                p_xbftMsg->GetReplicaAddress().c_str());
            return true;
        }

        // 获取本地保存的QC证书指针，是否已经初始化。
        XbftQcPointer &p_qc = p_node->GetSelfQc();
        if (p_qc == nullptr) {
            LOG_ERROR("Self qc isn't initialized(view:%ld, seq:%ld)", p_node->GetViewNumber(), p_node->GetSequence());
            break;
        }

        // 为QC证书添加VOTO消息中包含的签名数据
        if (!p_qc->Add(p_xbftMsg->GetReplicaId(), p_xbftMsg->GetXbftEnv().xbft().vote().qc_content(),
                p_xbftMsg->GetXbftEnv().xbft().vote().signature())) {
            LOG_ERROR("Failed to add new signature to qc");
            break;
        }

        // 保存接收到的投票消息记录
        if (!p_node->AddVote(p_xbftMsg->GetReplicaId())) {
            LOG_ERROR("duplicate vote for %s from %ld", utils::String::BinToHexString(nodeHash).c_str(),
                p_xbftMsg->GetReplicaId());
            break;
        }

        // 判断接收到的VOTE消息是否已经达到大多数，如果是，则更新树的状态。
        LOG_DEBUG("vote size(%ld), quorum size(%ld)", p_node->GetVotedSize(), GetQuorumSize());
        if (p_node->GetVotedSize() == GetQuorumSize()) {
            // 更新最新的节点及QC
            updateHighNodeAndQc(p_node, p_qc);

            //
            update(p_node, p_qc);

            // Received enough vote messages, send decide mesage
            XbftMsgPointer p_env = newDecide(p_node, 0);
            mp_net->m_sendMsg(
                mp_replicaKey->GetAddress(), GetOtherReplicaAddrs(), p_env->GetXbftEnv().SerializeAsString());
        }

        return true;
    } while (false);
    return false;
}

bool XbftConsensus::onPropose(const XbftMsgPointer &p_xbftMsg) {
    do {
        // Continue if it has only one node.
        if (IsLeader()) {
            LOG_WARN("Received PROPOSE message, leader would not process");
            return true;
        }

        if (GetLeaderId() != p_xbftMsg->GetReplicaId()) {
            LOG_WARN("PROPOSE message(id:%ld) is not from leader(%ld)", p_xbftMsg->GetReplicaId(), GetLeaderId());
            break;
        }

        // Check the value by ledger
        if (!mp_valueDealing->m_checkValue(p_xbftMsg->GetValue())) {
            LOG_ERROR("Failed to check xbft message(%s)", p_xbftMsg->GetDesc().c_str());
            break;
        }

        // Create a leaf node for PROPOSE mesaage
        XbftNodePointer p_node = std::make_shared<XbftNode>(p_xbftMsg->GetViewNumber(), p_xbftMsg->GetSeq(),
            p_xbftMsg->GetPreQc(), p_xbftMsg->GetParentHash(), p_xbftMsg->GetValue(), mp_keyTool);
        p_node->SetValidators(m_validators);

        // Update Qc Ref
        XbftNodePointer p_qcNode = m_nodeTree.GetNode(p_node->GetQc()->GetNodeHash());
        if (p_qcNode == nullptr) {
            if (p_node->GetSequence() == 2) {  // if it is second block, then ignore the qc
                LOG_INFO("Cann't find the qc node by hash(%s), ignore it as it is second block",
                    utils::String::Bin4ToHexString(p_node->GetQc()->GetNodeHash()).c_str());
            } else {
                LOG_ERROR("Cann't find the qc node by hash(%s), break",
                    utils::String::Bin4ToHexString(p_node->GetQc()->GetNodeHash()).c_str());
                break;
            }
        }

        if (p_qcNode != nullptr && !p_qcNode->CheckQc(p_node->GetQc())) {
            LOG_ERROR("Failed to check the qc values");
            break;
        }

        p_node->UpdateQcRef(p_qcNode);

        if (!m_nodeTree.AppendNode(p_node)) {
            LOG_ERROR("Failed to append new node to node tree");
            break;
        }

        // Update parent node
        update(p_node->GetQcRef(), p_node->GetQc());

        bool opinion = false;
        if (p_node->GetSequence() >= m_lastSequence) {  //>= or > may be considered for the future
            if (p_node->GetQcRef() && p_node->GetQcRef()->GetSequence() > mp_lock->GetSequence()) {
                opinion = true;  // liveness condition
                m_lastSequence = p_node->GetSequence();
            } else {  // safety condition (extend the locked branch)
                XbftNodePointer p_nodeTmp;
                for (p_nodeTmp = p_node; p_nodeTmp && p_nodeTmp->GetSequence() > mp_lock->GetSequence();
                     p_nodeTmp = m_nodeTree.GetParentNode(p_nodeTmp->GetHash()))
                    ;
                if (p_nodeTmp == mp_lock) { /* on the same branch */
                    opinion = true;
                    m_lastSequence = p_node->GetSequence();
                }

                if (!opinion) {
                    LOG_WARN("The node is not valid, node seq(%ld), last seq(%ld), lock seq(%ld)",
                        p_node->GetSequence(), m_lastSequence, mp_lock->GetSequence());
                }
            }
        } else {
            LOG_WARN(
                "The node sequence is not proper, node seq(%ld), last seq(%ld)", p_node->GetSequence(), m_lastSequence);
        }

        if (opinion) {
            XbftMsgPointer p_env =
                newVote(p_xbftMsg->GetXbftEnv().xbft().propose(), p_xbftMsg->GetValue()->GetHash(), 0);
            std::vector<std::string> vec;
            vec.push_back(getLeaderAddr());
            mp_net->m_sendMsg(mp_replicaKey->GetAddress(), vec, p_env->GetXbftEnv().SerializeAsString());
        } else {
            LOG_WARN("Replica(%ld) received unproper message", m_replicaId);
        }

        return true;
    } while (false);
    return false;
}

bool XbftConsensus::onDecide(const XbftMsgPointer &p_xbftMsg) {
    do {
        // Continue if it has only one node.
        if (IsLeader()) {
            LOG_WARN("Received PROPOSE message, leader would not process");
            return true;
        }

        if (GetLeaderId() != p_xbftMsg->GetReplicaId()) {
            LOG_WARN("PROPOSE message(id:%ld) is not from leader(%ld)", p_xbftMsg->GetReplicaId(), GetLeaderId());
            break;
        }

        // Create a leaf node for PROPOSE mesaage
        XbftQcPointer p_qc = std::make_shared<XbftQc>(p_xbftMsg->GetPreQc());

        // 查找该VOTE消息对应的树节点，是否存在。
        //  Update Qc Ref
        XbftNodePointer p_qcNode = m_nodeTree.GetNode(p_qc->GetNodeHash());
        if (p_qcNode == nullptr) {
            LOG_ERROR(
                "Cann't find the qc node by hash(%s)", utils::String::Bin4ToHexString(p_qc->GetNodeHash()).c_str());
            break;
        }

        // Check the qc
        // 核对QC是否有效
        if (!p_qcNode->CheckQc(p_qc)) {
            LOG_ERROR("Failed to check the qc values");
            break;
        }

        // 更新树状态
        update(p_qcNode, p_qc);
        return true;
    } while (false);
    return false;
}

bool XbftConsensus::update(const XbftNodePointer &p_blk2, const XbftQcPointer &p_qc) {
    /* Three step, p_nblk = b*, p_blk2 = b'', p_blk1 = b', p_blk = b */  // prepare,         pre-commit,    commited
                                                                         //                  mp_locked,   mp_exec
                                                                         // GetPreCommit:true             decide
    do {
        // const XbftNodePointer p_blk2 = p_nblk->GetQcRef();
        LOG_DEBUG("Update block status");
        if (p_blk2 == nullptr) {
            LOG_WARN("Failed to update qc, blk2 is null");
            break;
        }

        // decision，表示该block已经处理过，不需要继续处理
        if (p_blk2->GetDecision()) {
            LOG_INFO("No need to process, blk2 is decision");
            break;
        }

        // 该标志位表示，如果该区块没有预提交过，则将该区块预提交。
        if (!p_blk2->GetPreCommit()) {
            m_lastProof = generateProof(p_blk2, p_qc).SerializeAsString();
            mp_valueDealing->m_valueCommited(p_blk2->GetConsensusValue(), m_lastProof);
            LOG_INFO("Pre commit:%s", p_blk2->GetDesc().c_str());
            p_blk2->SetPreCommit(true);
            p_blk2->SetEndTime(utils::Timestamp::HighResolution());
        }

        const XbftNodePointer p_blk1 = p_blk2->GetQcRef();
        if (p_blk1 == nullptr) {
            LOG_WARN("Failed to update qc, blk1 is null");
            break;
        }
        if (p_blk1->GetDecision()) {
            break;
        }
        if (p_blk1->GetSequence() > mp_lock->GetSequence()) {
            mp_lock = p_blk1;
        }

        const XbftNodePointer p_blk = p_blk1->GetQcRef();
        if (p_blk == nullptr) {
            LOG_WARN("Failed to update qc, blk is null");
            break;
        }
        if (p_blk->GetDecision()) {
            break;
        }

        if (p_blk2->GetParentHash() != p_blk1->GetHash() || p_blk1->GetParentHash() != p_blk->GetHash()) {
            LOG_ERROR("Blk2's parent != Blk2's qc reference or Blk1's parent != Blk1's qc reference");
            break;
        }

        // p_blk及更老的node应该都为commited状态，找出这些node，将其
        std::vector<std::pair<XbftNodePointer, XbftQcPointer>> commitQueue;
        XbftNodePointer p_nodeTemp;
        XbftNodePointer p_preNodeTemp;
        for (p_nodeTemp = p_blk, p_preNodeTemp = p_blk1; p_nodeTemp->GetSequence() > mp_exec->GetSequence();
             p_preNodeTemp = p_nodeTemp, p_nodeTemp = m_nodeTree.GetNode(p_nodeTemp->GetParentHash())) {
            commitQueue.emplace_back(std::make_pair(p_nodeTemp, p_preNodeTemp->GetQc()));
        }
        if (p_nodeTemp != mp_exec) {
            LOG_ERROR("Safety breached, %s, %s", p_nodeTemp->GetDesc().c_str(), mp_exec->GetDesc().c_str());
            break;
        }

        for (auto it = commitQueue.rbegin(); it != commitQueue.rend(); it++) {
            const XbftNodePointer &p_node = it->first;
            p_node->SetDecision(true);
            LOG_INFO("Commit:%s", p_node->GetDesc().c_str());
        }


        mp_exec = p_blk;
        LOG_INFO(
            "id(%d) Exec node:%s, lock node:%s", m_replicaId, mp_exec->GetDesc().c_str(), mp_lock->GetDesc().c_str());
        return true;
    } while (false);

    return false;
}

void XbftConsensus::updateHighNodeAndQc(XbftNodePointer p_node, XbftQcPointer p_qc) {
    if (p_node->GetSequence() > m_highNodeAndQc.first->GetSequence()) {
        m_highNodeAndQc = std::make_pair(p_node, p_qc);
    }
}

protocol::XbftProof XbftConsensus::generateProof(const XbftNodePointer &p_nblk, const XbftQcPointer &p_qc) {
    protocol::XbftProof protoProof;
    protoProof.mutable_node()->set_view_number(p_nblk->GetViewNumber());
    protoProof.mutable_node()->set_sequence(p_nblk->GetSequence());
    protoProof.mutable_node()->set_parent_node_hash(p_nblk->GetParentHash());
    *protoProof.mutable_node()->mutable_previous_qc() = p_nblk->GetQc()->GetProtoQc();

    *protoProof.mutable_qc() = p_qc->GetProtoQc();
    return protoProof;
}
std::vector<std::string> XbftConsensus::GetOtherReplicaAddrs() const {
    std::vector<std::string> replicaAddrs;
    m_validators.GetOtherReplicaAddrs(replicaAddrs, m_replicaId);
    return replicaAddrs;
}

std::shared_ptr<KeyInterface> XbftConsensus::GetReplicaKey() {
    return mp_replicaKey;
}

int64_t XbftConsensus::GetReplicaId() const {
    return m_replicaId;
}

const XbftValidatorSet &XbftConsensus::GetValidators() const {
    return m_validators;
}

void XbftConsensus::ViewChanged(int64_t viewNumber, const XbftQcPointer &cr_qc) {
    m_viewNumber = viewNumber;
    m_isViewActive = true;
    LOG_INFO("Replica(%ld), View number changed to(%ld)", m_replicaId, m_viewNumber);
    mp_valueDealing->m_viewChange();
    do {
        if (!cr_qc) {
            break;
        }
        XbftNodePointer p_qcNode = m_nodeTree.GetNode(cr_qc->GetNodeHash());
        if (p_qcNode == nullptr) {
            LOG_WARN(
                "Cann't find the qc node by hash(%s)", utils::String::Bin4ToHexString(cr_qc->GetNodeHash()).c_str());
            break;
        }
        update(p_qcNode, cr_qc);
    } while (false);
}

void XbftConsensus::SetViewInActive() {
    LOG_INFO("Replica(%ld) View(%ld) is set to inactive", m_replicaId, m_viewNumber);
    m_isViewActive = false;
}

std::string XbftConsensus::getLeaderAddr() {
    int64_t leaderId = m_viewNumber % m_validators.Size();
    return m_validators.QueryAddr(leaderId);
}

bool XbftConsensus::IsViewActive() const {
    return m_isViewActive;
}

size_t XbftConsensus::GetQuorumSize() const {
    return m_validators.GetQuorumSize();
}

XbftQcPointer XbftConsensus::GetHighQc() {
    return m_highNodeAndQc.second;
}

XbftNodeTree &XbftConsensus::GetNodeTree() {
    return m_nodeTree;
}
}  // namespace xbft
