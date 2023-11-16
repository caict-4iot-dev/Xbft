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
 * @file: XbftNode.h
 */

#ifndef __XBFTNODE_H__
#define __XBFTNODE_H__

#include "XbftQc.h"
#include "XbftValidatorSet.h"
#include "consensus.pb.h"
#include <map>
#include <string>

namespace xbft {
struct KeyToolInterface;
class XbftNode;
using XbftNodePointer = std::shared_ptr<XbftNode>;
using XbftEnvPointer = std::shared_ptr<protocol::XbftEnv>;
const int64_t g_xbft_instance_timeout_ = 30 * 1000 * 1000;  // 3 second
class ConsData;
class XbftNode {
public:
    XbftNode(int64_t viewNumber, int64_t sequence, std::shared_ptr<KeyToolInterface> p_keyTool);
    XbftNode(int64_t viewNumber, int64_t sequence, const protocol::XbftQc &cr_protoQc, const std::string &cr_parentHash,
        std::shared_ptr<ConsData> p_consValue, std::shared_ptr<KeyToolInterface> p_keyTool);
    XbftNode(const protocol::XbftNode &cr_node, std::shared_ptr<KeyToolInterface> p_keyTool);
    ~XbftNode();

    const std::string &GetHash() const;
    const std::string &GetParentHash() const;
    const int64_t &GetViewNumber() const;
    const int64_t &GetSequence() const;
    const protocol::XbftNode &GetProtoNode() const;
    void UpdateQcRef(XbftNodePointer p_node);
    const XbftNodePointer &GetQcRef() const;
    bool GetDecision() const;
    void SetDecision(bool decision);
    bool GetPreCommit() const;
    void SetPreCommit(bool commited);
    std::string GetDesc() const;
    std::shared_ptr<ConsData> GetConsensusValue();

    static size_t GetQuorumSize(size_t size);
    void SetProposeMsg(XbftEnvPointer p_msg);
    const XbftQcPointer &GetQc() const;

    size_t GetVotedSize() const;
    bool AddVote(int64_t replica);
    XbftQcPointer &GetSelfQc();
    void InitSelfQc();
    void SetValidators(const XbftValidatorSet &cr_validators);

    bool NeedSendProposeAgain(int64_t current_time) const;
    void SetLastProposeTime(int64_t current_time, int64_t round);
    int64_t GetProposeRound() const;
    void SetEndTime(int64_t currentTime);
    void SetHash(const std::string &cr_hash);

    bool CheckQc(const XbftQcPointer &cr_selfQc) const;

private:
    int64_t m_viewNumber;
    int64_t m_sequence;

    protocol::XbftNode m_node;
    std::shared_ptr<ConsData> mp_consValue;
    std::unordered_set<int64_t> m_voted;
    XbftQcPointer mp_qc;      // for previous qc
    XbftQcPointer mq_selfQc;  // for this instance qc
    bool m_decision;
    bool m_preCommit;          // for temprary pre commited
    XbftNodePointer mp_qcRef;  // the last node

    // derive from above value
    std::string m_hash;
    std::string m_parentHash;

    // record the time
    int64_t m_startTime;
    int64_t m_lastProposeTime;
    int64_t m_endTime;

    XbftEnvPointer mp_propseMsg;
    int64_t m_proposeRound;

    XbftValidatorSet m_validators;

    std::shared_ptr<KeyToolInterface> mp_keyTool;
};

}  // namespace xbft
#endif  //__XBFTNODE_H__