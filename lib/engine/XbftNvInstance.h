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
 * @file: XbftNvInstance.h
 */

#ifndef __XBFTNVINSTANCE_H__
#define __XBFTNVINSTANCE_H__

#include "consensus.pb.h"

namespace xbft {
class XbftNvInstance {
public:
    XbftNvInstance();
    XbftNvInstance(int64_t viewNumber);
    XbftNvInstance(const XbftNvInstance &ins);
    ~XbftNvInstance();

    int64_t GetViewNumber() const;
    bool AddNewView(int64_t replicaId, const protocol::XbftNewView &cr_protoNewView);
    size_t GetVotedSize() const;
    bool NeedSendAgain(int64_t currentTime, int64_t interval) const;  // judge if we should send again?
    void SetLastSendTime(int64_t currentTime);
    void ViewChangeComplete(int64_t currentTime);
    bool IsViewChangeComplete() const;
    protocol::XbftTc GetTc();
    protocol::XbftQc GetHighQc();

    int64_t GetLastSendTime() const;

private:
    int64_t m_viewNumber;

    // record the time
    int64_t m_startTime;
    int64_t m_lastSendTime;
    int64_t m_endTime;

    std::map<int64_t, protocol::XbftNewView> m_newViewProto;
};
using XbftNvInstanceMap = std::map<int64_t, XbftNvInstance>;
}  // namespace xbft
#endif