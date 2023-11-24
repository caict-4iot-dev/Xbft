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
 * @date: 2023-11-23 11:23:22
 * @file: Network.h
 */

#ifndef NET_NETWORK_H_
#define NET_NETWORK_H_

#include <iostream>
#include <string>
#include "common/Configure.h"
#include "common/EventQueue.h"
#include "common/Singleton.h"
#include <thread>
#include <map>

namespace net {

	class Network : public common::Singleton<net::Network> 
	{
		friend class common::Singleton<net::Network>;
	public:
		Network();
		~Network();

        bool Initialize();
		bool Exit();
	
		void SendMsg(const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value);
		common::EventQueue<std::string> &GetMsgQueue();

	private:
		void recvMsg();
		
	private:
		int m_nodeSocket; 

		std::map<std::string, std::string> m_bootnode;

		common::EventQueue<std::string> m_recvMsgQueue;

		std::thread * mp_recvThread;

		char * recvBuf;
	};
}

#endif