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
 * @file: XbftQc.h
 */

#ifndef __XBFTQC_H__
#define __XBFTQC_H__

#include "consensus.pb.h"
#include <string>

namespace xbft {
class XbftQc {
public:
    XbftQc(int64_t viewNumber, int64_t sequence, const std::string &cr_nodeValueHash);  // construct, view number or
                                                                                        // sequence may be not used
    XbftQc(const protocol::XbftQc &cr_protoQc);
    ~XbftQc();

    bool Add(int64_t replicaId, const protocol::XbftQcValue &cr_qcValue, const protocol::Signature &cr_sig);
    size_t GetSignatureSize() const;            // Get the signature's counter of qc
    const std::string &GetContentHash() const;  // Get the node hash of qc
    std::string GetNodeHash() const;
    const protocol::XbftQc &GetProtoQc() const;
    std::string GetProtoQcString() const;

private:
    protocol::XbftQc m_qc;
    std::string m_contentHash;
};

using XbftQcPointer = std::shared_ptr<XbftQc>;
}  // namespace xbft
#endif