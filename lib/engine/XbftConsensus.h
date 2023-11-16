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
 * @file: XbftConsensus.h
 */

#ifndef __XBFTCONSENSUS_H__
#define __XBFTCONSENSUS_H__

#include "ConsEngine.h"
#include "XbftMsg.h"
#include "XbftNodeTree.h"
#include "XbftValidatorSet.h"
#include <vector>


namespace xbft {
class XbftConsensus {
public:
    XbftConsensus(std::shared_ptr<NetInterface> p_net, std::shared_ptr<ValueDealInterface> p_valueDeal, std::shared_ptr<KeyToolInterface> p_keyTool);
    ~XbftConsensus();

    bool Initialize(std::shared_ptr<KeyInterface> p_keyInfo, const std::vector<std::string> &cr_validatorsVec,
        int64_t viewNumber, const std::string &cr_lastProof);  // Initialize this object with replica's private key and
                                                               // total validators
    bool Exit();                                               // Execute before this object exits
    bool Propose(std::shared_ptr<ConsData> p_consData);        // It is a portal which called by ledger module for
                                                               // consensus
    bool OnRecv(const XbftMsgPointer &p_xbftMsg);  // It is a portal which called by p2p module when p2p received xbft
                                                   // messages
    std::string GetCurrentLeader() const;          // Get current consensus leader's address
    bool IsLeader() const;                         // Return true if this node is a leader
    bool IsValidator() const;                      // Return true if this node is a leader
    bool IsViewActive() const;
    size_t GetQuorumSize() const;  // Get quorum size, 2f+1
    bool CheckProof(const std::string &cr_proof, const std::function<void(bool)> &cr_fun);
    bool UpdateValidatorsAndProof(const protocol::ValidatorSet &cr_validatorSet, const std::string &cr_lastProof);
    void UpdateValidators(const protocol::ValidatorSet &cr_validatorSet);
    bool UpdateProof(const std::string &proof);
    int64_t GetViewNumber() const;
    int64_t GetLeaderId() const;
    std::vector<std::string> GetOtherReplicaAddrs() const;
    std::shared_ptr<KeyInterface> GetReplicaKey();
    int64_t GetReplicaId() const;
    const XbftValidatorSet &GetValidators() const;
    void ViewChanged(int64_t viewNumber, const XbftQcPointer &quc);
    void SetViewInActive();
    XbftQcPointer GetHighQc();
    XbftNodeTree &GetNodeTree();
    bool SendPropseAgain(const XbftNodePointer &p_node);

private:
    void loadValues();
    XbftMsgPointer newPropose(const XbftNodePointer &p_node, int64_t round = 0) const;  // New a propose message object
    XbftMsgPointer newVote(const protocol::XbftPropose &propose, const std::string &nodeValueHash,
        int64_t roundNumber) const;                                                    // New a vote message object
    XbftMsgPointer newDecide(const XbftNodePointer &p_node, int64_t round = 0) const;  // New a decide message object
    bool onPropose(const XbftMsgPointer &p_xbftMsg);                        // Execute when received propose message
    bool onVote(const XbftMsgPointer &p_xbftMsg);                           // Execute when received vote message
    bool onDecide(const XbftMsgPointer &p_xbftMsg);                         // Execute when received decide message
    bool update(const XbftNodePointer &p_nblk, const XbftQcPointer &p_qc);  // Update the node tree status when a
                                                                            // process achieved
    void updateHighNodeAndQc(XbftNodePointer p_node, XbftQcPointer);
    protocol::XbftProof generateProof(const XbftNodePointer &p_nblk, const XbftQcPointer &p_qc);
    std::string getLeaderAddr();
    void initSelfQc(const XbftNodePointer &p_node);

private:
    std::shared_ptr<ValueDealInterface> mp_valueDealing;
    std::shared_ptr<NetInterface> mp_net;
    std::shared_ptr<KeyToolInterface> mp_keyTool;

    XbftNodeTree m_nodeTree;
    int64_t m_viewNumber;
    int64_t m_lastSequence;

    XbftValidatorSet m_validators;
    bool m_isViewActive;
    std::shared_ptr<KeyInterface> mp_replicaKey;
    int64_t m_replicaId;
    size_t m_faultNumber;

    std::pair<XbftNodePointer, XbftQcPointer> m_highNodeAndQc;
    XbftNodePointer mp_lock;
    XbftNodePointer mp_exec;  // must be initialize
};

}  // namespace xbft

#endif