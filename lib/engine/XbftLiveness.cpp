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
 * @file: XbftLiveness.cpp
 */


#include "XbftLiveness.h"
#include "ConsEngine.h"
#include "Logger.h"
#include "Timestamp.h"
#include "XbftTc.h"


namespace xbft {
const int64_t g_newviewMessageOffset = 5;

RoundRobinPaceMaker::RoundRobinPaceMaker(std::shared_ptr<NetInterface> p_net,
    std::shared_ptr<KeyToolInterface> p_keyTool, std::shared_ptr<XbftConsensus> p_consensus) {
    mp_consensus = p_consensus;
    mp_net = p_net;
    mp_keyTool = p_keyTool;
}

RoundRobinPaceMaker::~RoundRobinPaceMaker() = default;

bool RoundRobinPaceMaker::Initialize(int64_t sendMsgInterval) {
    m_sendMsgInterval = sendMsgInterval;
    m_timerLoop.SetTimeOut(1000, [this]() { onTimeout(); });
    return true;
}

bool RoundRobinPaceMaker::Exit() {
    m_timerLoop.Stop();
    return true;
}

bool RoundRobinPaceMaker::Rotate() {
    if (mp_consensus->GetReplicaId() < 0) {
        LOG_INFO("Replica(%ld) is not consensus node, ignore it", mp_consensus->GetReplicaId());
        return true;
    }

    // 首先设置共识的状态为非活跃状态
    mp_consensus->SetViewInActive();

    // 创建New View的消息对象
    XbftMsgPointer p_newView = newNewView(mp_consensus->GetViewNumber() + 1);

    // Self receive the new view
    // 首先由本节点处理该New View消息
    onNewView(p_newView, true);

    // Send new view message to others
    // 然后发送该New View消息到其他的节点
    mp_net->m_sendMsg(mp_consensus->GetReplicaKey()->GetAddress(), mp_consensus->GetOtherReplicaAddrs(),
        p_newView->GetXbftEnv().SerializeAsString());
    LOG_INFO(
        "Replica(%ld), Leader rotate, Send new view(%ld)", mp_consensus->GetReplicaId(), p_newView->GetViewNumber());
    return true;
}

bool RoundRobinPaceMaker::OnRecv(const XbftMsgPointer &p_xbftMsg) {
    do {
        LOG_INFO("Replica(%ld), Receive (%s) message", mp_consensus->GetReplicaId(), p_xbftMsg->GetDesc().c_str());
        // Check message's format and signature
        if (!p_xbftMsg->CheckMsgItem(mp_consensus->GetValidators())) {
            LOG_ERROR("Failed to check xbft message");
            break;
        }

        return onNewView(p_xbftMsg, false);
    } while (false);

    return false;
}

bool RoundRobinPaceMaker::onNewView(const XbftMsgPointer &p_xbftMsg, bool selfMsg) {
    do {
        const protocol::XbftNewView &newView = p_xbftMsg->GetXbftEnv().xbft().new_view();
        // 判断NewView消息是否包含Tc，如果包含则意味着有可能网络已经达成了新View的共识
        if (newView.has_last_tc() && newView.last_tc().view_number() > mp_consensus->GetViewNumber()) { /*check last tc
                                                                                                           signature*/
            XbftTc tcc(newView.last_tc(), mp_keyTool);
            // 判断该TC是否有效，如果有效则表示网络已经对新View已达成共识
            if (!tcc.IsValid(mp_consensus->GetValidators(), mp_consensus->GetQuorumSize())) {
                LOG_ERROR("Failed to check the last tc from message");
                break;
            }

            // should set consensus to last tc view number
            LOG_INFO("Replica(%ld),Received last tc, set consensus view number to last tc's number(%ld)",
                mp_consensus->GetReplicaId(), newView.last_tc().view_number());

            // consensus共识对象处理视图切换成功的操作。
            mp_consensus->ViewChanged(newView.last_tc().view_number(), nullptr);
        }

        // 如果该NewView消息不包含Tc，则判断该ViewNumber是不是比当前的view更新
        if (p_xbftMsg->GetViewNumber() <= mp_consensus->GetViewNumber()) {
            LOG_ERROR("Replica(%ld),The view number(%ld) recieved <= current view(%ld)", mp_consensus->GetReplicaId(),
                p_xbftMsg->GetViewNumber(), mp_consensus->GetViewNumber());
            break;
        }

        auto iter = m_nvMap.find(p_xbftMsg->GetViewNumber());
        if (iter == m_nvMap.end()) {
            LOG_TRACE("Not find new view(%ld) instance, will create it", p_xbftMsg->GetViewNumber());
            m_nvMap.insert(std::make_pair(p_xbftMsg->GetViewNumber(), XbftNvInstance(p_xbftMsg->GetViewNumber())));
            iter = m_nvMap.find(p_xbftMsg->GetViewNumber());
        }

        // 将该NewView消息加入到NvInstance实例当中
        XbftNvInstance &ins = iter->second;
        if (!ins.AddNewView(p_xbftMsg->GetReplicaId(), p_xbftMsg->GetXbftEnv().xbft().new_view())) {
            LOG_WARN("Duplicate recv new view message from replica(%ld)", p_xbftMsg->GetReplicaId());
            break;
        }

        // 判断该操作是否是自己触发的，则设置一下NewView消息的最新发送时间
        if (selfMsg) {
            ins.SetLastSendTime(utils::Timestamp::HighResolution());
        }

        // 判断收集到的VOTE消息数量是否达到大多数。
        if (ins.GetVotedSize() >= mp_consensus->GetQuorumSize()) {
            LOG_INFO("Replica(%ld),Received enough new view message, view number will be changed to number(%ld)",
                mp_consensus->GetReplicaId(), p_xbftMsg->GetViewNumber());
            // notify the consensus object
            // 达到大多数，代表视图切换成功，则通知consensus对象
            mp_consensus->ViewChanged(p_xbftMsg->GetViewNumber(), std::make_shared<XbftQc>(ins.GetHighQc()));

            // upgrade the nv instance
            // 更新NV实例的视图切换完成时间
            ins.ViewChangeComplete(utils::Timestamp::HighResolution());

            // upgrade local last tc
            // 更新本地最新的TC证书
            mp_lastTc = std::make_shared<protocol::XbftTc>(ins.GetTc());
        }

        return true;
    } while (false);

    return false;
}

void RoundRobinPaceMaker::onTimeout() {
    // Delete the older new view instance
    // 检查NV实例，如果时间太久则删掉
    for (auto iter = m_nvMap.begin(); iter != m_nvMap.end();) {
        const XbftNvInstance &instance = iter->second;
        if (instance.GetViewNumber() < mp_consensus->GetViewNumber() - g_newviewMessageOffset) {
            m_nvMap.erase(iter++);
        } else {
            iter++;
        }
    }

    // Check if we need to send the new view message again?
    // 检查各个NV实例，是否要再次发送NV消息
    int64_t currentTime = utils::Timestamp::HighResolution();
    for (auto &item : m_nvMap) {
        XbftNvInstance &ins = item.second;
        if (ins.GetLastSendTime() == 0) {
            continue;
        }

        if (ins.NeedSendAgain(currentTime, m_sendMsgInterval)) {
            LOG_INFO("Replica(%ld),Need to send new view again, view(%ld), last(%ld), interval(%ld)",
                mp_consensus->GetReplicaId(), ins.GetViewNumber(), ins.GetLastSendTime(), m_sendMsgInterval);
            XbftMsgPointer p_newView = newNewView(ins.GetViewNumber(), 0);
            mp_net->m_sendMsg(mp_consensus->GetReplicaKey()->GetAddress(), mp_consensus->GetOtherReplicaAddrs(),
                p_newView->GetXbftEnv().SerializeAsString());
            ins.SetLastSendTime(currentTime);
        }
    }

    // 检查各个Cosensus实例，是否要再次发送Propose消息
    std::string hash = mp_consensus->GetNodeTree().GetHighestLeafHash();
    XbftNodePointer p_node = mp_consensus->GetNodeTree().GetNode(hash);
    if (p_node && p_node->NeedSendProposeAgain(currentTime) && mp_consensus->IsViewActive()) {
        LOG_INFO("Need send propose msg again, node(%s) ", p_node->GetDesc().c_str());
        mp_consensus->SendPropseAgain(p_node);
    }
}

XbftMsgPointer RoundRobinPaceMaker::newNewView(int64_t viewNumber, int64_t roundNumber) const {
    XbftEnvPointer p_env = std::make_shared<protocol::XbftEnv>();
    protocol::Xbft *p_xbft = p_env->mutable_xbft();
    p_xbft->set_round_number(roundNumber);
    p_xbft->set_type(protocol::XBFT_TYPE_NEWVIEW);
    protocol::XbftNewView *p_newView = p_xbft->mutable_new_view();
    p_newView->set_replica_id(mp_consensus->GetReplicaId());
    if (mp_lastTc) {
        *p_newView->mutable_last_tc() = *mp_lastTc;
    }

    protocol::XbftTcValue *tcValue = p_newView->mutable_tc_content();

    tcValue->set_view_number(viewNumber);
    int64_t highQcViewNumber = 0;
    if (mp_consensus->GetHighQc()) {
        *tcValue->mutable_high_qc() = mp_consensus->GetHighQc()->GetProtoQc();
        highQcViewNumber = tcValue->high_qc().qc_content().view_number();
    }

    char strSignedViewNumber[2 * sizeof(int64_t)];
    *(int64_t *)strSignedViewNumber = viewNumber;
    *(int64_t *)(strSignedViewNumber + sizeof(int64_t)) = highQcViewNumber;
    p_newView->mutable_signature()->set_public_key(mp_consensus->GetReplicaKey()->GetPublicKey());
    p_newView->mutable_signature()->set_sign_data(
        mp_consensus->GetReplicaKey()->Sign(std::string(strSignedViewNumber)));

    p_env->mutable_signature()->set_public_key(mp_consensus->GetReplicaKey()->GetPublicKey());
    p_env->mutable_signature()->set_sign_data(mp_consensus->GetReplicaKey()->Sign(p_xbft->SerializeAsString()));
    return std::make_shared<XbftMsg>(*p_env, mp_keyTool);
}

XbftTcPointer RoundRobinPaceMaker::GetLastTc() {
    return mp_lastTc;
}

}  // namespace xbft
