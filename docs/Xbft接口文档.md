

## 接口列表

| 序号 | 接口                     | 说明                             |
| ---- | ------------------------ | -------------------------------- |
| 1    | CreateXbftEngine         | 创建xbft引擎                     |
| 2    | Rotate                   | 触发XBFT共识视图更改             |
| 3    | UpdateValidatorsAndProof | 更新共识验证器和证明信息         |
| 4    | Propose                  | 发起提案                         |
| 5    | Recv                     | 接收和处理xbft共识协议数据       |
| 6    | IsLeader                 | 检测是否为主节点                 |
| 7    | GetLatestProof           | 获取最新共识证明数据             |
| 8    | GetViewNumber            | 获取最新共识视图序号             |
| 9    | IsViewActive             | 检查当前节点是否正在进行视图切换 |
| 10   | GetQuorumSize            | 获取xbft共识QUORUM号             |

## 接口介绍

### CreateXbftEngine

> 描述

创建xbft引擎



> 接口

```c++
std::shared_ptr<BftEngine> CreateXbftEngine(std::shared_ptr<NetInterface> p_net,
    								std::shared_ptr<ValueDealInterface> p_valueDeal, 
									std::shared_ptr<NodeInfoInterface> p_nodeInfo,
   								    std::shared_ptr<KeyToolInterface> p_keyTool);
```



> 参数

| 参数        | 类型                                | 说明             |
| ----------- | ----------------------------------- | ---------------- |
| p_net       | std::shared_ptr<NetInterface>       | 网络相关接口     |
| p_valueDeal | std::shared_ptr<ValueDealInterface> | 数据处理相关接口 |
| p_nodeInfo  | std::shared_ptr<NodeInfoInterface>  | 节点信息相关接口 |
| p_keyTool   | std::shared_ptr<KeyToolInterface>   | key相关接口      |




```c++
//定义一个名为SendMsgFun函数指针类型。发送共识数据到其他共识节点
//@param cr_from：发送共识数据的节点地址
//@param cr_dest：接收共识数据的节点列表
//@param cr_value：共识数据
typedef void (*SendMsgFun)(const std::string &cr_from, const std::vector<std::string> &cr_dest, const std::string &cr_value);

struct NetInterface {
    SendMsgFun m_sendMsg;
};
```



```c++
//定义一个名为CheckValueFun函数指针类型。检查待共识数据是否有效
//@param p_consData：待共识数据
typedef bool (*CheckValueFun)(std::shared_ptr<ConsData> p_consData);

//定义一个名为ValueCommitedFun函数指针类型。提交待共识数据
//@param p_consData：待共识数据
//@param cr_proof: 共识证明信息
typedef void (*ValueCommitedFun)(std::shared_ptr<ConsData> p_consData, const std::string &cr_proof);

//定义一个名为ViewChangeFun函数指针类型。视图转换
typedef void (*ViewChangeFun)();

struct ValueDealInterface {
    CheckValueFun m_checkValue;
    ValueCommitedFun m_valueCommited;
    ViewChangeFun m_viewChange;
};
```



```c++
class NodeInfoInterface {
public:
    virtual std::vector<std::string> GetValidators() = 0;  //获取共识节点列表
    virtual std::string GetLastProof() = 0;  //获取最新共识证明数据
    virtual int64_t GetLastViewNumber() = 0;  //获取最新视图序号
    virtual std::shared_ptr<KeyInterface> GetKeyInfo() = 0; //获取key
    virtual int64_t GetSendMsgInterval() = 0;  //获取共识消息发送间隔
};
```



```c++
//定义一个名为VerifyFun函数指针类型。签名验证
typedef bool (*VerifyFun)(const std::string &cr_signData, const std::string   			                                          &cr_signature, const std::string &cr_publicKey);

//定义一个名为PublicKeyToAddrFun函数指针类型。公钥转节点地址
typedef std::string (*PublicKeyToAddrFun)(const std::string &cr_publicKey);

//定义一个名为StringToConsDataFun函数指针类型。共识数据格式化
typedef std::shared_ptr<ConsData> (*StringToConsDataFun)(const std::string &cr_input);

struct KeyToolInterface {
    VerifyFun m_verify;
    PublicKeyToAddrFun m_publicKeyToAddr;
    StringToConsDataFun m_createConsData;
};
```



> 返回值

返回xbft引擎实例



###   Rotate

> 描述

触发XBFT共识视图更改



> 接口

```c++
void Rotate(std::shared_ptr<BftEngine> p_engine);
```



> 参数

| 参数     | 类型                       | 说明     |
| -------- | -------------------------- | -------- |
| p_engine | std::shared_ptr<BftEngine> | xbft引擎 |



### UpdateValidatorsAndProof

> 描述

当数据更新时，我们可以使用这个函数来更新共识验证器和证明信息



> 接口

```c++
bool UpdateValidatorsAndProof(std::shared_ptr<BftEngine> p_engine, const std::vector<std::string> &cr_validators, const std::string &cr_lastProof);
```



> 参数

| 参数          | 类型                             | 说明             |
| ------------- | -------------------------------- | ---------------- |
| p_engine      | std::shared_ptr<BftEngine>       | xbft引擎         |
| cr_validators | const std::vector< std::string > | 共识节点地址列表 |
| cr_lastProof  | const std::string                | 最新共识证明信息 |



> 返回值

返回布尔类型，true更新成功，false更新失败



### Propose

> 描述

提供共识数据并发起xbft共识提案



> 接口

```c++
bool Propose(std::shared_ptr<BftEngine> p_engine, std::shared_ptr<ConsData> p_consData);
```



> 参数

| 参数       | 类型                       | 说明     |
| ---------- | -------------------------- | -------- |
| p_engine   | std::shared_ptr<BftEngine> | xbft引擎 |
| p_consData | std::shared_ptr<ConsData>  | 共识数据 |



> 返回值

返回布尔类型，true提案发起成功，false提案发起失败



### Recv

> 描述

接收和处理xbft共识协议数据



> 接口

```c++
void Recv(std::shared_ptr<BftEngine> p_engine, const std::string &cr_consMsg, 	 std::shared_ptr<KeyToolInterface> p_keyTool);
```



> 参数

| 参数       | 类型                              | 说明               |
| ---------- | --------------------------------- | ------------------ |
| p_engine   | std::shared_ptr<BftEngine>        | xbft引擎           |
| cr_consMsg | const std::string                 | 接收的共识协议数据 |
| p_keyTool  | std::shared_ptr<KeyToolInterface> | key相关接口        |



### IsLeader

> 描述

检测当前节点是否为xbft共识中的主节点



> 接口

```c++
bool IsLeader(std::shared_ptr<BftEngine> p_engine);
```



> 参数

| 参数     | 类型                       | 说明     |
| -------- | -------------------------- | -------- |
| p_engine | std::shared_ptr<BftEngine> | xbft引擎 |



> 返回值

返回布尔类型



###   GetLatestProof

> 描述

获取最新共识的xbft证明数据



> 接口

```c++
std::string GetLatestProof(std::shared_ptr<BftEngine> p_engine);
```



> 参数

| 参数     | 类型                       | 说明     |
| -------- | -------------------------- | -------- |
| p_engine | std::shared_ptr<BftEngine> | xbft引擎 |



### GetViewNumber

> 描述

获取最新共识的xbft视图序号



>接口

```c++
int64_t GetViewNumber(std::shared_ptr<BftEngine> p_engine);
```



> 参数

| 参数     | 类型                       | 说明     |
| -------- | -------------------------- | -------- |
| p_engine | std::shared_ptr<BftEngine> | xbft引擎 |



### IsViewActive

> 描述

检查当前节点是否正在进行视图切换



> 接口

```c++
bool IsViewActive(std::shared_ptr<BftEngine> p_engine);
```



> 参数

| 参数     | 类型                       | 说明     |
| -------- | -------------------------- | -------- |
| p_engine | std::shared_ptr<BftEngine> | xbft引擎 |



###   GetQuorumSize

> 描述

获取xbft共识QUORUM号



> 接口

```c++
size_t GetQuorumSize(std::shared_ptr<BftEngine> p_engine);
```



> 参数

| 参数     | 类型                       | 说明     |
| -------- | -------------------------- | -------- |
| p_engine | std::shared_ptr<BftEngine> | xbft引擎 |

