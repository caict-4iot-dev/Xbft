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
 * @date: 2023-11-15 14:23:43
 * @file: Xbft.h
 */

#include "ConsEngine.h"

#ifndef __XBFT_H__
#define __XBFT_H__
namespace xbft {
/**
 * @description: create xbft engine pointer
 * @return {*}
 */
std::shared_ptr<BftEngine> CreateXbftEngine(std::shared_ptr<NetInterface> p_net,
    std::shared_ptr<ValueDealInterface> p_valueDeal, std::shared_ptr<NodeInfoInterface> p_nodeInfo,
    std::shared_ptr<KeyToolInterface> p_keyTool);

/**
 * @description: trigger xbft consensus view-change
 * @param {shared_ptr<BftEngine>} p_engine
 * @return {*}
 */
void Rotate(std::shared_ptr<BftEngine> p_engine);

/**
 * @description: when data update, we can use this function to update consensus validators and proof infomation
 * @return {true: update success. false:update fail}
 */
bool UpdateValidatorsAndProof(const std::vector<std::string> &cr_validators, const std::string &cr_lastProof);

/**
 * @description: Provide consensus data and initiate xbft consensus proposal
 * @param {shared_ptr<BftEngine>} p_engine
 * @param {shared_ptr<ConsData>} p_consData
 * @return {true: propose success. false:propose fail}
 */
bool Propose(std::shared_ptr<BftEngine> p_engine, std::shared_ptr<ConsData> p_consData);

/**
 * @description: Receive and process xbft consensus protocol data
 * @param {const std::string &} cr_consMsg
 * @param {shared_ptr<KeyToolInterface>} p_keyTool
 * @return {true: propose success. false:propose fail}
 */
void Recv(
    std::shared_ptr<BftEngine> p_engine, const std::string &cr_consMsg, std::shared_ptr<KeyToolInterface> p_keyTool);

/**
 * @description: Detects whether the current node is the primary node in the xbft consensus
 * @param {shared_ptr<BftEngine>} p_engine
 * @return {true: is leader. false: not leader}
 */
bool IsLeader(std::shared_ptr<BftEngine> p_engine);

/**
 * @description: Get xbft proof data for the most recent consensus
 * @param {shared_ptr<BftEngine>} p_engine
 * @return {proof data}
 */
std::string GetLatestProof(std::shared_ptr<BftEngine> p_engine);

/**
 * @description: Gets the xbft view-number data for the most recent consensus
 * @param {shared_ptr<BftEngine>} p_engine
 * @return {view-number}
 */
int64_t GetViewNumber(std::shared_ptr<BftEngine> p_engine);

/**
 * @description: Check whether view switching is underway on the current node
 * @param {shared_ptr<BftEngine>} p_engine
 * @return {true: not in view-change; false: in view-change}
 */
bool IsViewActive(std::shared_ptr<BftEngine> p_engine);


/**
 * @description: Obtain the xbft consensus QUORUM number
 * @param {shared_ptr<BftEngine>} p_engine
 * @return {quorum num}
 */
size_t GetQuorumSize(std::shared_ptr<BftEngine> p_engine);

}  // namespace xbft


#endif
