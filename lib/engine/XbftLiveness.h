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
 * @file: XbftLiveness.h
 */


#ifndef __XBFTLIVENESS_H__
#define __XBFTLIVENESS_H__

#include "Timer.h"
#include "XbftConsensus.h"
#include "XbftNvInstance.h"
#include "consensus.pb.h"
#include <map>
#include <memory>
#include <string>


namespace xbft {

using XbftTcPointer = std::shared_ptr<protocol::XbftTc>;
class RoundRobinPaceMaker {
public:
    RoundRobinPaceMaker(std::shared_ptr<NetInterface> p_net, std::shared_ptr<KeyToolInterface> p_keyTool,
        std::shared_ptr<XbftConsensus> p_consensus);
    ~RoundRobinPaceMaker();

    bool Initialize(int64_t sendMsgInterval);  // Start a timeout
    bool Exit();

    bool Rotate();                                 // When block timeout, this function will be trigger
    bool OnRecv(const XbftMsgPointer &p_xbftMsg);  // Receive new view message
    XbftTcPointer GetLastTc();

private:                                                                           // replica
    XbftMsgPointer newNewView(int64_t viewNumber, int64_t roundNumber = 0) const;  // Create the new view message
    void onTimeout();                                                              // Timeout event
    bool onNewView(const XbftMsgPointer &p_xbftMsg, bool selfMsg);

private:
    XbftNvInstanceMap m_nvMap;                    // Store all the new view messages by view number
    std::shared_ptr<XbftConsensus> mp_consensus;  // Pointer to the consensus object
    std::shared_ptr<NetInterface> mp_net;         // Pointer to the notify object
    std::shared_ptr<KeyToolInterface> mp_keyTool;

    utils::TimerLoop m_timerLoop;
    int64_t m_sendMsgInterval;  // The interval of sending new view message
    XbftTcPointer mp_lastTc;
};

}  // namespace xbft
#endif