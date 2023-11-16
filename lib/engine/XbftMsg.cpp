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
 * @file: XbftMsg.cpp
 */


#include "XbftMsg.h"
#include "ConsEngine.h"
#include "Logger.h"
#include "Sha256.h"
#include "Strings.h"

namespace xbft {

XbftMsg::XbftMsg(const protocol::XbftEnv &cr_xbftEnv, std::shared_ptr<KeyToolInterface> p_keyTool) {
    m_xbftEnv = cr_xbftEnv;
    mp_keyTool = p_keyTool;
    GetBasicInfo(cr_xbftEnv, m_strType, m_viewNumber, m_sequence, m_replicaId, m_previousQc, m_parentHash);
    std::string strValue;
    if (AnalysisValue(cr_xbftEnv, strValue)) {
        m_value = std::make_shared<ConsData>(strValue);
    }
    m_replicaAddr = GetReplicaAddress(m_xbftEnv);
    m_hash = crypto::Sha256::Crypto(m_xbftEnv.SerializeAsString());
}

XbftMsg::~XbftMsg() = default;

void XbftMsg::GetBasicInfo(const protocol::XbftEnv &cr_xbftEnv, std::string &r_type, int64_t &r_viewNumber,
    int64_t &r_seq, int64_t &r_replicaId, protocol::XbftQc &r_preQc, std::string &r_parentHash) {
    r_replicaId = 0;
    const protocol::Xbft &xbft = xbftEnv.xbft();
    switch (xbft.type()) {
    case protocol::XBFT_TYPE_PROPOSE: {
        r_type = "PROPOSE";
        if (xbft.has_propose()) {
            r_seq = xbft.propose().node().sequence();
            r_viewNumber = xbft.propose().node().view_number();
            r_replicaId = xbft.propose().replica_id();
            r_preQc = xbft.propose().node().previous_qc();
            r_parentHash = xbft.propose().node().parent_node_hash();
        }
        break;
    }
    case protocol::XBFT_TYPE_VOTE: {
        r_type = "VOTE";
        if (xbft.has_vote()) {
            r_seq = xbft.vote().qc_content().sequence();
            r_viewNumber = xbft.vote().qc_content().view_number();
            r_replicaId = xbft.vote().replica_id();
        }
        break;
    }
    case protocol::XBFT_TYPE_DECIDE: {
        r_type = "DECIDE";
        if (xbft.has_decide()) {
            r_seq = xbft.decide().qc().qc_content().sequence();
            r_viewNumber = xbft.decide().qc().qc_content().view_number();
            r_replicaId = xbft.decide().replica_id();
            r_preQc = xbft.decide().qc();
        }
        break;
    }
    case protocol::XBFT_TYPE_NEWVIEW: {
        r_type = "NEWVIEW";
        r_seq = 0;
        r_replicaId = 0;
        if (xbft.has_new_view() && xbft.new_view().has_tc_content()) {
            r_viewNumber = xbft.new_view().tc_content().view_number();
            r_replicaId = xbft.new_view().replica_id();
        }
        break;
    }
    default: {
        break;
    }
    }
}

std::string XbftMsg::GetReplicaAddress(const protocol::XbftEnv &cr_xbftEnv) {
    return mp_keyTool->PublicKeyToAddr(cr_xbftEnv.signature().public_key());
}


bool XbftMsg::AnalysisValue(const protocol::XbftEnv &cr_xbftEnv, std::string &r_value) {
    const protocol::Xbft &xbft = cr_xbftEnv.xbft();
    if (xbft.type() == protocol::XBFT_TYPE_PROPOSE) {
        r_value = xbft.propose().node().node_value();
        return true;
    }
    return false;
}

bool XbftMsg::operator<(const XbftMsg &msg) const {
    return m_hash < msg.GetHash();
}
bool XbftMsg::operator==(const XbftMsg &valueFrm) const {
    return m_hash == valueFrm.GetHash();
}

int64_t XbftMsg::GetSeq() const {
    return m_sequence;
}

int64_t XbftMsg::GetViewNumber() const {
    return m_viewNumber;
}

std::shared_ptr<ConsData> XbftMsg::GetValue() {
    return m_value;
}

const protocol::XbftQc &XbftMsg::GetPreQc() const {
    return m_previousQc;
}

const std::string &XbftMsg::GetReplicaAddress() const {
    return m_replicaAddr;
}

int64_t XbftMsg::GetReplicaId() const {
    return m_replicaId;
}

const std::string &XbftMsg::GetStrType() const {
    return m_strType;
}

const protocol::XbftEnv &XbftMsg::GetXbftEnv() const {
    return m_xbftEnv;
}

const std::string &XbftMsg::GetHash() const {
    return m_hash;
}

const std::string &XbftMsg::GetParentHash() const {
    return m_parentHash;
}

size_t XbftMsg::GetSize() const {
    return (size_t)m_xbftEnv.ByteSize();
}


protocol::XbftMessageType XbftMsg::GetType() const {
    return m_xbftEnv.xbft().type();
}

std::string XbftMsg::GetDesc() const {
    return utils::String::Format("addr:%s,type:%s,view:%ld,seq:%ld,hash:%s", GetReplicaAddress().c_str(),
        m_strType.c_str(), m_viewNumber, m_sequence, utils::String::Bin4ToHexString(GetHash()).c_str());
}

bool XbftMsg::CheckMsgItem(const XbftValidatorSet &cr_validators) const {
    int64_t rid = cr_validators.QueryId(m_replicaAddr);
    do {
        if (rid == -1) {
            LOG_ERROR("Unable to find validator(%s) from list", m_replicaAddr.c_str());
            break;
        }

        if (rid != m_replicaId) {
            LOG_ERROR("The message replica id(%ld) is not equal to the signature id(%ld)", rid, m_replicaId);
            break;
        }

        if (!m_xbftEnv.has_xbft() || !m_xbftEnv.has_signature()) {
            LOG_ERROR("Failed to check the object of xbft or signature");
            break;
        }

        bool ret = true;
        const protocol::Xbft &xbft = m_xbftEnv.xbft();
        switch (xbft.type()) {
        case protocol::XBFT_TYPE_PROPOSE: {
            if (!xbft.has_propose() || !xbft.propose().has_node()) {
                ret = false;
            }
            break;
        }
        case protocol::XBFT_TYPE_VOTE: {
            if (!xbft.has_vote() || !xbft.vote().has_signature()) {
                ret = false;
            } else {  // check the signatrue
                const protocol::XbftQcValue &qcValue = xbft.vote().qc_content();

                if (!mp_keyTool->Verify(qcValue.SerializeAsString(), xbft.vote().signature().sign_data(),
                        xbft.vote().signature().public_key())) {
                    ret = false;
                }
            }
            break;
        }
        case protocol::XBFT_TYPE_DECIDE: {
            if (!xbft.has_decide()) {
                ret = false;
            }
            break;
        }
        case protocol::XBFT_TYPE_NEWVIEW: {
            if (!xbft.has_new_view()) {
                ret = false;
            } else {  // check new view
                const protocol::XbftTcValue &tcValue = xbft.new_view().tc_content();
                int64_t highQcViewNumber = 0;
                if (tcValue.has_high_qc()) {
                    highQcViewNumber = tcValue.high_qc().qc_content().view_number();
                }
                char strSignedViewNumber[2 * sizeof(int64_t)];
                *(int64_t *)strSignedViewNumber = tcValue.view_number();
                *(int64_t *)(strSignedViewNumber + sizeof(int64_t)) = highQcViewNumber;
                if (!mp_keyTool->Verify(strSignedViewNumber, xbft.new_view().signature().sign_data(),
                        xbft.new_view().signature().public_key())) {
                    ret = false;
                }
            }
            break;
        }
        default:
            break;
        }

        if (!ret) {
            LOG_ERROR("Failed to check the format");
            break;
        }

        const protocol::Xbft &pbft = m_xbftEnv.xbft();
        const protocol::Signature &sig = m_xbftEnv.signature();
        if (!mp_keyTool->Verify(pbft.SerializeAsString(), sig.sign_data(), sig.public_key())) {
            LOG_TRACE("ser:%s", utils::String::BinToHexString(pbft.SerializeAsString()).c_str());
            LOG_TRACE("sig:%s", utils::String::BinToHexString(sig.sign_data()).c_str());
            LOG_TRACE("pub:%s", utils::String::BinToHexString(sig.public_key()).c_str());
            LOG_ERROR("Failed to check received message's signature, desc(%s)", GetDesc().c_str());
            break;
        }

        return true;
    } while (false);

    return false;
}

}  // namespace xbft
