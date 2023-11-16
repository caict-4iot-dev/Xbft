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
 * @file: XbftNvInstance.cpp
 */

#include "XbftNvInstance.h"
#include "Logger.h"
#include "Timestamp.h"

namespace xbft {
XbftNvInstance::XbftNvInstance() = default;
XbftNvInstance::XbftNvInstance(int64_t viewNumber) {
    m_viewNumber = viewNumber;
    m_startTime = utils::Timestamp::HighResolution();
    m_lastSendTime = 0;
    m_endTime = 0;
}

XbftNvInstance::XbftNvInstance(const XbftNvInstance &cr_ins) {
    m_viewNumber = cr_ins.m_viewNumber;
    m_startTime = cr_ins.m_startTime;
    m_lastSendTime = cr_ins.m_lastSendTime;
    m_endTime = cr_ins.m_endTime;
    m_newViewProto = cr_ins.m_newViewProto;
}

XbftNvInstance::~XbftNvInstance() = default;

int64_t XbftNvInstance::GetViewNumber() const {
    return m_viewNumber;
}

bool XbftNvInstance::AddNewView(int64_t replicaId, const protocol::XbftNewView &protoNewView) {
    if (protoNewView.tc_content().view_number() != m_viewNumber) {
        assert(false);  // Should not be called
    }

    return m_newViewProto.insert(std::make_pair(replicaId, protoNewView)).second;
}

size_t XbftNvInstance::GetVotedSize() const {
    return m_newViewProto.size();
}

bool XbftNvInstance::NeedSendAgain(int64_t currentTime, int64_t interval) const {
    return ((currentTime - m_lastSendTime >= interval) &&
            (m_endTime == 0 || utils::Timestamp::HighResolution() < m_endTime + 60 * 1000 * 1000));
}

void XbftNvInstance::SetLastSendTime(int64_t currentTime) {
    m_lastSendTime = currentTime;
    LOG_INFO("Set last send time(%ld)", m_lastSendTime);
}

void XbftNvInstance::ViewChangeComplete(int64_t currentTime) {
    m_endTime = currentTime;
}

bool XbftNvInstance::IsViewChangeComplete() const {
    return m_endTime > 0;
}

int64_t XbftNvInstance::GetLastSendTime() const {
    return m_lastSendTime;
}

protocol::XbftTc XbftNvInstance::GetTc() {
    protocol::XbftTc tc;
    tc.set_view_number(m_viewNumber);
    for (auto item : m_newViewProto) {
        const protocol::XbftQc &qc = item.second.tc_content().high_qc();
        tc.add_high_qc_view_number(qc.qc_content().view_number());
        *tc.add_signature() = item.second.signature();
    }

    return tc;
}

protocol::XbftQc XbftNvInstance::GetHighQc() {
    protocol::XbftQc itemQc;
    itemQc.mutable_qc_content()->set_sequence(0);
    for (auto item : m_newViewProto) {
        const protocol::XbftQc &qc = item.second.tc_content().high_qc();
        if (qc.qc_content().sequence() > itemQc.qc_content().sequence()) {
            itemQc = qc;
        }
    }

    return itemQc;
}
}  // namespace xbft
