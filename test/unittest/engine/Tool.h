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
 * @date: 2023-11-21 17:00:20
 * @file: Tool.h
 */

#ifndef __TOOL_H__
#define __TOOL_H__

#include "ConsEngine.h"
#include "XbftEngine.h"
#include "XbftMsg.h"
#include "consensus.pb.h"
#include <gtest/gtest.h>
#include <mutex>


namespace xbft {
class XbftEngine;
class ConsData;
using XbftMsgPointer = std::shared_ptr<XbftMsg>;
using XbftEnvPointer = std::shared_ptr<protocol::XbftEnv>;
}  // namespace xbft

class XbftEngineMsg {
public:
    std::string m_fromAddr;
    std::vector<std::string> m_toAddrs;
    std::string msg;
};

class XbftEngineValueCommited {
public:
    std::shared_ptr<xbft::ConsData> value;
    std::string proof;
};

static std::vector<XbftEngineMsg> g_msgs;
static std::mutex g_msgLock;
static std::vector<XbftEngineValueCommited> g_valueCommited;

class ConsensusValue : public xbft::ConsData {
public:
    int64_t GetSeq() { return m_seq; };
    std::string GetPreviousProof() { return m_previousProof; };
    std::string GetHash() { return m_hash; };
    std::string GetStringValue() { return m_stringValue; };

public:
    int64_t m_seq;
    std::string m_previousProof;
    std::string m_hash;
    std::string m_stringValue;
};

class Key : public xbft::KeyInterface {
public:
    std::string GetPrivateKey() { return m_pri; };
    std::string GetPublicKey() { return m_pub; };
    std::string GetAddress() { return m_address; };
    std::string Sign(const std::string &cr_signData) {
        std::string signature = m_pri + m_pub + m_address;
        return signature;
    };

    void SetPri(const std::string &cr_pri) { m_pri = cr_pri; };
    void SetPub(const std::string &cr_pub) { m_pub = cr_pub; };
    void SetAdd(const std::string &cr_add) { m_address = cr_add; };

public:
    std::string m_pri;
    std::string m_pub;
    std::string m_address;
};

class tmpNodeInfo : public xbft::NodeInfoInterface {
public:
    std::vector<std::string> GetValidators() {
        std::vector<std::string> vec;
        vec.clear();
        vec.push_back("address-0");
        vec.push_back("address-1");
        vec.push_back("address-2");
        vec.push_back("address-3");
        return vec;
    };
    std::string GetLastProof() { return m_lastProof; };
    int64_t GetLastViewNumber() { return m_viewNumber; };
    std::shared_ptr<xbft::KeyInterface> GetKeyInfo() { return mp_key; };
    int64_t GetSendMsgInterval() { return m_interval; };

public:
    std::shared_ptr<xbft::KeyInterface> mp_key;
    int64_t m_interval;
    std::string m_lastProof;
    int64_t m_viewNumber;
};

class toolMock {
public:
    static void sendMsg(
        const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value) {
        std::lock_guard<std::mutex> guard(g_msgLock);
        g_msgs.push_back(XbftEngineMsg{cr_from, cr_dest, cr_value});
    }

    static bool verify(
        const std::string &cr_signData, const std::string &cr_signature, const std::string &cr_publicKey) {
        return true;
    }

    static bool verifyFalse(
        const std::string &cr_signData, const std::string &cr_signature, const std::string &cr_publicKey) {
        return false;
    }

    static std::string pubKeyToAddress(const std::string &cr_publicKey) {
        if (cr_publicKey == "pub-0")
            return "address-0";
        if (cr_publicKey == "pub-1")
            return "address-1";
        if (cr_publicKey == "pub-2")
            return "address-2";
        if (cr_publicKey == "pub-3")
            return "address-3";
        return "";
    }

    static std::shared_ptr<xbft::ConsData> createConsData(const std::string &cr_input) {
        return std::make_shared<ConsensusValue>();
    }

    static bool checkValue(std::shared_ptr<xbft::ConsData> p_consData) { return true; }

    static void valueCommited(std::shared_ptr<xbft::ConsData> p_consData, const std::string &cr_proof) {
        do {
            std::lock_guard<std::mutex> guard(g_msgLock);
            g_valueCommited.push_back(XbftEngineValueCommited{p_consData, cr_proof});
        } while (false);
    }

public:
};

class Tool : public testing::Test {
public:
    static std::vector<XbftEngineMsg> FetchMsgAndClear();
    static std::vector<XbftEngineValueCommited> FetchCommitedAndClear();
    static std::shared_ptr<xbft::NetInterface> CreateNet();
    static std::shared_ptr<xbft::KeyToolInterface> CreateKeyTool();
    static std::shared_ptr<xbft::ValueDealInterface> CreateValueDeal();
    static std::shared_ptr<tmpNodeInfo> CreateNodeInfo(std::shared_ptr<xbft::KeyInterface> p_key, int64_t interVal);
    static std::shared_ptr<Key> CreateKeyInfo(const std::string &index);
};

#endif