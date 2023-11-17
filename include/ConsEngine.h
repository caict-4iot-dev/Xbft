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
 * @date: 2023-11-15 14:28:34
 * @file: ConsEngine.h
 */

#ifndef __CONS_ENGINE_H__
#define __CONS_ENGINE_H__

#include <memory>
#include <string>
#include <vector>

namespace xbft {
// cons data struct;
class ConsData {
public:
    virtual int64_t GetSeq() = 0;
    virtual std::string GetPreviousProof() = 0;
    virtual std::string GetHash() = 0;
    virtual std::string GetStringValue() = 0;
};

// net interface for cons engine to exchange data
typedef void (*SendMsgFun)(
    const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value);

struct NetInterface {
    SendMsgFun SendMsg;
};


// key interface
class KeyInterface {
public:
    virtual std::string GetPrivateKey() = 0;
    virtual std::string GetPublicKey() = 0;
    virtual std::string GetAddress() = 0;
    virtual std::string Sign(const std::string &cr_signData) = 0;
};

// key tool
typedef bool (*VerifyFun)(
    const std::string &cr_signData, const std::string &cr_signature, const std::string &cr_publicKey);
typedef std::string (*PublicKeyToAddrFun)(const std::string &cr_publicKey);
typedef std::shared_ptr<ConsData> (*StringToConsDataFun)(const std::string &cr_input);

struct KeyToolInterface {
    VerifyFun Verify;
    PublicKeyToAddrFun PublicKeyToAddr;
    StringToConsDataFun CreateConsData;
};

// config info to set node info
class NodeInfoInterface {
public:
    virtual std::vector<std::string> GetValidators() = 0;
    virtual std::string GetLastProof() = 0;
    virtual int64_t GetLastViewNumber() = 0;
    virtual std::shared_ptr<KeyInterface> GetKeyInfo() = 0;
    virtual int64_t GetSendMsgInterval() = 0;
};

// value dealing
typedef bool (*CheckValueFun)(std::shared_ptr<ConsData> p_consData);
typedef void (*ValueCommitedFun)(std::shared_ptr<ConsData> p_consData, const std::string &cr_proof);

struct ValueDealInterface {
    CheckValueFun CheckValue;
    ValueCommitedFun ValueCommited;
};

class BftEngine {
public:
    virtual std::string GetEngineName() = 0;     // engine name
    virtual std::string GetEngineVersion() = 0;  // engine version
    virtual bool StartEngine(std::shared_ptr<NodeInfoInterface> p_nodeInfo) = 0;

public:
    virtual void Rotate() = 0;
    virtual bool Propose(std::shared_ptr<ConsData> p_consData) = 0;
    virtual void Recv(const std::string &cr_msg, std::shared_ptr<KeyToolInterface> p_keyTool) = 0;
    virtual bool IsLeader() = 0;
    virtual std::string GetLatestProof() = 0;
    virtual int64_t GetViewNumber() = 0;
};

}  // namespace xbft

#endif
