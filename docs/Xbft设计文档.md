# Xbft项目设计文档

## 总体设计

### 总体描述

#### 算法描述

​	Xbft共识模块，基于hotstuff流水线共识算法实现；是一种优化后的三阶段BFT算法，当拜占庭节点数小于共识节点数的1/3时，可以保证系统的安全、高效运行、并提供状态的最终性保证；Xbft当前仅支持共识节点数 >= 4的场景，

​	1) 当创世节点数小于4时，节点无法成功启动；

​    2) 当链上共识节点数量动态减少时，导致链上节点数小于4的交易，执行失败。

XBFT在实现过程中，发现当每次都切换leader，下一个被切换的leader可能还没有获取到最新的状态，所以可能下一个共识不能成功，导致共识的稳定性存在问题；因此，根据情况对hotstuff流水线共识算法的基础上进行了如下方面的优化：

​	1) leader节点不在每次投票后主动切换；

​    2) new-view仅由超时进行触发

​    3) 不再有空块 Dummy block的概念

​    4) 节点广播并收集new-view消息，所有节点共同判断2f+1个new-view后进行视图切换

#### 项目目的

​	依托Xbft共识，输出Xbft共识算法库

### 逻辑架构

![逻辑架构](.\image\xbft-逻辑架构.jpg)

逻辑架构如上图，从功能上看，可分为以上几个独立模块；

1)   数据输入：负责向共识核心逻辑提供待共识数据及leader切换信号；并提供Xbft共识数据处理需要的数据验证、数据处理方法；

2)   Xbft共识数据处理：Xbft核心业务逻辑；分为业务消息处理与活性消息处理两个模块；业务消息处理负责hotstuff共识中的提案、投票、决策消息的发送与处理，进行共识数据验证、共识数据处理操作；另外，还进行QC管理、共识树管理等逻辑。活性消息管理，负责接收leader切换信号，完成hotstuff共识算法中的出块主节点切换。

3)   网络模块：与Xbft逻辑进行数据交互，进行共识中的数据单播/广播及数据接收；

4)   数据输出：待共识数据经过共识核心逻辑处理后，将输出待共识数据的proof数据；并且当发生视图切换时，将输出最新的view-number

### 模块结构

#### 模块结构图

![模块结构](.\image\xbft-项目模块结构.jpg)

Xbft采用层级架构设计。分为应用层、核心逻辑层、通用工具层。

1)应用层：实现Xbft引擎启动管理。

​	接入管理：引擎启动依赖项及引擎对外操作管理。如网络消息、配置消息等；

​	共识管理引擎：依托依赖管理模块，构建Xbft共识引擎；

2) 核心逻辑层：Xbft流水线共识核心实现逻辑，采用活性与安全性逻辑解耦方式实现。

​	节点树管理：对hotstuff共识中构建的临时树进行管理；

​	验证者数据管理：hotstuff为BFT类共识算法，需要验证者信息用于共识节点身份的确认；

​	视图切换数据管理：搜集视图切换的new-view数据；

​	QC管理：QC相关操作；

​	活性管理：节点存在异常时，触发主节点切换相关流程；

​	共识管理：流水线共识相关逻辑，包括提案、投票、数据处理等相关操作；

3) 链通用工具层：包括日志管理、密码算法工具、通用工具等；

#### 代码文件划分

> Xbft
>
> ├── build
>
> ├── cmake
>
> ├── docs
>
> ├── include
>
> ├── lib
>
> │  ├── engine
>
> │  ├── protos
>
> │  └── utils
>
> ├── test
>
> │  ├── sample
>
> │  └── unittest
>
> ├──.clang-format
>
> ├──.clang-tidy
>
> ├──.gitignore
>
> ├── ChangeLog.txt
>
> └── README.md

1）build/: 项目编译目录，各种编译的临时文件和最终的目标文件皆存于此

2）cmake/: 项目依赖文件编译cmake文件

3）docs/: 保存项目各种文档

4）include/: 保存项目头文件

5）lib/: 项目代码文件

6）utils/: 其它通用模块文件

​	 engine/: Xbft共识算法核心逻辑

​	 protos/：Xbft共识算法数据结构

7）test/: 项目测试文件

​	sample/：代码使用样例文件

​	unittest/: 模块单元测试文件 

#### 整体类图

![类图](.\image\xbft-类图.jpg)

#### 线程架构

![线程架构](.\image\xbft-线程架构.jpg)

Xbft中仅存在一个由定时器触发的线程；

Xbft将数据处理流程注册到网络模块后，由网络模块触发调用；

### 数据结构定义

Xbft核心数据结构皆采用protobuf3语法进行定义，以方便在不同的语言之间进行通信。

```
message XbftNode {
    bytes parent_node_hash = 1; //父节点hash
    bytes node_value = 2;    //该节点包含的区块数据（即consensus value）
    int64 view_number = 3; //当前视图的view_number
    int64 sequence = 4;  //当前的共识实例的高度，一般等于区块高度
    XbftQc previous_qc = 5; //上一共识的QC集合，由leader通知给replica用
} 


message XbftQcValue {           //QC证书value
    bytes node_value_hash = 1;  // 共识数据hash
    bytes node_hash = 2;        // hotstuff node hash
    int64 view_number = 3;     
    int64 sequence = 4;          
}

message XbftQc {//QC证书，在共识中一个比较重要的概念，对应星火BFT算法中的proof证据概念
    XbftQcValue qc_content = 1;  // { parent_node_hash, node_value_hash, view_number, sequence, previous_qc}
    repeated Signature signatures = 2;
}

enum XbftMessageType {
    XBFT_TYPE_PROPOSE = 0; //提案
    XBFT_TYPE_VOTE = 1;    //投票
    XBFT_TYPE_DECIDE = 2;  //决策
    XBFT_TYPE_NEWVIEW = 3;//新视图
}

message XbftPropose {          //该消息只由leader节点发出
    int64 replica_id = 1;       //区块链节点的id 
    XbftNode node = 2;         //hotstuff的树节点
}

message XbftVote {
    int64 replica_id = 1;    //区块链节点的id 
    XbftQcValue qc_content = 2;
    Signature signature = 3;   //对Proposal消息的签名 // { parent_node_hash, node_value_hash, view_number, sequence, previous_qc}
}

message XbftDecide {
    int64 replica_id = 1;
    XbftQc qc = 2;
}

message XbftTcValue {
    int64 view_number = 1;
    XbftQc high_qc = 2;    //high qc
}

message XbftTc {
    int64 view_number = 1;
    repeated int64 high_qc_view_number = 2;   //high_qc的view_number列表?or sequence?
    repeated Signature signature = 3;   //对应的view_number 和 high_qc_view_number的签名
}

message XbftNewView {
    int64 replica_id = 1;    //区块链节点的id 
    XbftTc last_tc = 2; //最后的TC证书
    XbftTcValue tc_content = 3;
    Signature signature = 4; //对tc_content.view_number和tc_content.high_qc.view_number进行签名。最后由节点自行组装成XbftTc
}

message Xbft {
    int64 round_number = 1; 
    XbftMessageType  type = 2;
    oneof XbftMsg {
        XbftPropose propose = 3;
        XbftVote vote = 4;
        XbftDecide decide = 5;
        XbftNewView new_view = 6;
    }
}

message XbftEnv {
    Xbft xbft = 1;
    Signature signature = 2;
}

message Validator{
	string address = 1;
}

message ValidatorSet{
	repeated Validator validators = 1;
}

message XbftProof {
    XbftNode node = 1;  //attention, node.node_value == ""
    XbftQc qc = 2;
}

message Signature {
	string public_key = 1;
    oneof data{
        bytes sign_data = 2;
        string sign_data_str = 3;
    }
}
```

### 三方库

```
googletest:
https://github.com/google/googletest.git
openssl:
https://github.com/openssl/openssl.git
ed25519:
https://github.com/floodyberry/ed25519-donna.git
protobuf:
https://github.com/protocolbuffers/protobuf
glog:
https://github.com/google/glog.git
gflags:
https://github.com/gflags/gflags
yaml:
https://github.com/jbeder/yaml-cpp
```

## 核心模块设计解析

### 接口抽象

![接口抽象](.\image\xbft-接口抽象.jpg)

#### Xbft提供接口

```
Xbft.h
#include "consEngine.h"

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
bool UpdateValidatorsAndProof(std::shared_ptr<BftEngine> p_engine, const std::vector<std::string> &cr_validators, const std::string &cr_lastProof);

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
```

```
consEngine.h
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
    SendMsgFun m_sendMsg;
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
    VerifyFun m_verify;
    PublicKeyToAddrFun m_publicKeyToAddr;
    StringToConsDataFun m_createConsData;
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
typedef void (*ViewChangeFun)();

struct ValueDealInterface {
    CheckValueFun m_checkValue;
    ValueCommitedFun m_valueCommited;
    ViewChangeFun m_viewChange;
};

class BftEngine {
public:
    virtual std::string GetEngineName() = 0;     // engine name
    virtual std::string GetEngineVersion() = 0;  // engine version
    virtual bool StartEngine(std::shared_ptr<NodeInfoInterface> p_nodeInfo) = 0;
};

}  // namespace xbft
```

#### XbftEngine实现

```
xbftEngine.h
#include “consEngine.h”
namespace xbft 
{
class XbftEngine : public BftEngine {
public:
    XbftEngine(std::shared_ptr<NetInterface> p_net, std::shared_ptr<ValueDealInterface> p_valueDeal,
        std::shared_ptr<KeyToolInterface> p_keyTool);
    ~XbftEngine();

public:
    std::string GetEngineName();
    std::string GetEngineVersion();
    bool StartEngine(std::shared_ptr<NodeInfoInterface> p_nodeInfo);

public:
    void Rotate(); //触发视图切换
    bool Propose(std::shared_ptr<ConsData> p_consData); //发起提案
    void Recv(const std::string &cr_consMsg, std::shared_ptr<KeyToolInterface> p_keyTool); //xbft模块消息接收

    bool IsLeader(); //判断当前节点是否为主节点
    std::string GetLatestProof(); //获取当前节点保存的最新区块的proof数据（QC）
    int64_t GetViewNumber(); //获取共识当前节点保存的view-number
    bool IsViewActive(); //判断当前节点是否在正常共识
    size_t GetQuorumSize(); //获取节点quorum size
    bool UpdateValidatorsAndProof(const std::vector<std::string> &cr_validators, const std::string &cr_lastProof);  //更新节点验证者列表及proof数据

private:
    std::shared_ptr<XbftConsensus> mp_consensus;
    std::shared_ptr<RoundRobinPaceMaker> mp_paceMaker;
};} //namespace xbft
```

#### 模块通信

目前涉及跨模块通信的为网络模块与待共识数据处理模块；

在XbftEngine初始化时，会将对应模块对象传入引擎；

### 共识流程说明

#### 正常共识流程

![共识](.\image\xbft-正常共识流程.jpg)

正常共识流程描述如下，

1)   共识主节点提供待共识消息，并调用共识模块Propose方法；

2)   共识模块主节点进行判断，判断当前轮次是否为主节点，并构建propose消息，发送到网络模块进行广播；

3)   共识模块从节点接收网络模块中的propose消息(新propose生成，更新QC)

​	a)    回调CheckValue进行待共识消息的验证；

​	b)   更新共识节点树状态，并进行propose消息的验证；

​	c)    如合法，则构建并发送vote消息到网络模块，单播到主节点；

4)   共识模块主节点接收网络模块中的vote消息，等待并收集2f+1条vote消息后，进行下一步操作；**(主节点prepareQC形成，检查2-chain;3-chain的状态)**

​	a)    主节点构建QC证书，更新共识节点树状态，将上+1轮共识数据通过调用回调OnValueCommited进行preCommit；上+2轮的数据	更新为commited；上+3轮以前的数据更新为decide; 

​	b)   构建decide消息，并发送到网络模块进行广播；

5)   共识模块从节点接收网络模块中的decide消息(从节点接收主节点的QC，**从节点prepareQC形成，检查2-chain;3-chain的状态**)

​	a)    检查QC状态

​	b)    更新共识节点树状态;

```
关于update的理解：
一个区块中的QC是对其直接父区块的确认，那么我们称之为1-chain；每当一个新的区块形成，节点都会检查是否会形成1-chain，2-chian，3-chain.
	1-chain: 有新的prepareQC形成，更新本地的prepareQC
	2-chain: 有新的precommitQC形成，更新本地的lockedQC
	3-chian: 有新的commitQC形成，有新的区块分支进入commit状态，执行确认的区块分支
```

#### 视图切换流程

Xbft中的视图切换流程进行了优化；利用超时触发机制，节点广播new-view消息；各个节点各自接收并处理new-view消息。节点等待并收集2f+1条new-view消息后，触发节点视图切换

![视图切换](.\image\xbft-视图切换流程.jpg)

视图切换流程如下：

1)   节点生成新区块超时，则触发视图切换；其调用活性模块，并传递view-number+1作为新视图序号；

2)   活性模块接收到视图切换信号进行处理；

​	a)    调用共识模块，设置节点状态为共识切换；

​	b)   生成new-view消息，节点自己进行处理；

​	c)    将new-new消息发送到网络模块进行广播；

3)   其它共识节点接收网络模块的new-view消息

​	a)    等待并收集2f+1个new-view消息；

​	b)   通知共识模块完成view-change切换

​         i.     共识模块接收调用，设置view-number

​         ii.     结束节点共识切换状态；

​        iii.     更新共识节点树状态

4)   活性模块除了处理new-view消息外，节点维护了定时器；

​	a)    删除低水位new-view消息集合；

​	b)   触发new-view重发机制，保证视图切换稳定性；

​	c）触发当前view-number下的主节点进行propose消息重发