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
 * @file: XbftMsg.h
 */

#ifndef __XBFTMSG_H__
#define __XBFTMSG_H__

#include "XbftNode.h"
#include "consensus.pb.h"
#include <memory>

namespace xbft {

struct KeyToolInterface;

class XbftMsg {
public:
    XbftMsg(const protocol::XbftEnv &cr_xbftEnv, std::shared_ptr<KeyToolInterface> p_keyTool);
    ~XbftMsg();

    bool operator<(const XbftMsg &cr_msg) const;
    bool operator==(const XbftMsg &cr_valueFrm) const;
    int64_t GetSeq() const;
    std::shared_ptr<ConsData> GetValue();          // Get xbft message's consensus value
    const std::string &GetReplicaAddress() const;  // Get replica's address
    int64_t GetReplicaId() const;
    const std::string &GetStrType() const;        // Get this message's type
    const protocol::XbftEnv &GetXbftEnv() const;  // Get reference of this proto object
    const std::string &GetHash() const;           // Get this message's hash
    const protocol::XbftQc &GetPreQc() const;
    size_t GetSize() const;  // Get this message's size
    std::string GetDesc() const;
    protocol::XbftMessageType GetType() const;
    int64_t GetViewNumber() const;
    const std::string &GetParentHash() const;


    bool CheckMsgItem(const XbftValidatorSet &cr_validators) const;  // Check the format
    static void GetBasicInfo(const protocol::XbftEnv &cr_xbftEnv, std::string &r_type, int64_t &r_viewNumber,
        int64_t &r_seq, int64_t &r_replicaId, protocol::XbftQc &r_preQc, std::string &r_parentHash);
    static std::string GetReplicaAddress(const protocol::XbftEnv &cr_xbftEnv);
    static bool AnalysisValue(const protocol::XbftEnv &cr_xbftEnv, std::string &r_value);

private:
    std::shared_ptr<KeyToolInterface> mp_keyTool;
    protocol::XbftEnv m_xbftEnv;

    // derived from above value
    int64_t m_viewNumber;
    int64_t m_sequence;
    int64_t m_replicaId;
    std::string m_strType;
    std::string m_replicaAddr;
    std::string m_hash;
    std::string m_parentHash;
    protocol::XbftQc m_previousQc;
    std::shared_ptr<ConsData> m_value;
};

using XbftMsgPointer = std::shared_ptr<XbftMsg>;
}  // namespace xbft
#endif
