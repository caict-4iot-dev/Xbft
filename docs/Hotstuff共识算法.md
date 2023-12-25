# Hotstuff共识算法

​	HotStuff共识算法是一个三阶段投票的BFT类共识协议，它总结了PBFT、Tendermint等共识算法的特点，实现了一个既有安全性（safety）、活性（liveness），又有响应性（responsiveness）的共识算法。

HotStuff的核心围绕着三轮共识投票展开，原论文中提出了三种形式:

```
简易版HotStuff（Basic HotStuff）
链状HotStuff（Chained HotStuff）
事件驱动的HotStuff（Event-Driven HotStuff）
```

## 基本概念

### 门限签名

​	一个（k，n）门限签名方案指由n个成员组成的签名群体，所有成员共同拥有一个公共密钥，每个成员拥有各自的私钥。只要收集到k个成员的签名，且生成一个完整的签名，该签名可以通过公钥进行验证。

### 证书

​	主节点收到至少quorum个节点对用一个提案的投票消息（带签名）后，利用门限签名将其合成一个QC，这个QC可以理解为门限签名生成的完整签名，表示对该次提案达成一次共识

### 视图

​	视图是共识的基本单元，一个视图至多达成一次共识，并且单调递增，每个视图逐渐轮换推进共识。

### 共识状态树

​	每个共识区块可以看做是一个树节点，每个树节点内包含对应的提案内容（前文的操作指令）和相对应的QC，每个树节点包含一个父亲树节点的哈希，形成一棵树状结构，主节点基于本地最长的分支生成新的树节点。

![image-20231206150754397](.\image\image-20231206150754397.png)

## **Basic** **Hotstuff** 

​	每个视图内都有一个唯一的主节点负责提案、收集和转发消息并生成QC，整个过程包括4个阶段：准备阶段（PREPARE）、预提交阶段（PRE-COMMIT）、提交阶段（COMMIT）、决定阶段（DECIDE），主节点提交（达成共识）某个分支，在PREPARE、PRE-COMMIT、COMMIT三个阶段收集quorum个共识节点带签名的投票消息，利用门限签名合成一个QC，然后广播给其他节点。

​	HotStuff 结合门限签名可以将之前互相广播共识消息的方式，转为由主节点处理、合并转发，通信复杂度可以降低到o(n)，简而言之就是HotStuff用门限签名+两轮通信达到了PBFT一轮通信的共识效果。

![image-20231206150900726](.\image\image-20231206150900726.png)

### PREPARE阶段

​	主节点：

   1）根据收到的quorum条New-View消息，该消息中包含了发送方的状态树中高度最高的prepareQC，主节点在收到的prepareQC中计算出高度最高的QC，记为highQC；

   2）根据这个highQC的节点所指向的分支，打包区块创建新的树节点，其父节点为highQC指向的节点；

   3）将生成的提案附带在prepare消息中发送给其他从节点，且当前提案包含highQC。

​	![image-20231206150950330](.\image\image-20231206150950330.png)

​	从节点： 

   1）收到该prepare消息之后，对prepare中的信息进行验证，包括qc中签名的合法性；是否当前视图的提案；

   2）prepare消息中的节点是否扩展自lockedQC的分支（即孩子节点）或者prepare消息中的highQC的视图号大于lockedQC（前者为安全条件，后者为活性条件保证节点落后时及时同步）；

   3）生成prepare-vote消息并附带一个签名发送给主节点。

![image-20231206151050333](.\image\image-20231206151050333.png)

![image-20231206151104740](.\image\image-20231206151104740.png)

​	验证规则

   1）Proposal消息中的区块是从本机lockQC的区块扩展产生（即m.block是lockQC.block的子孙区块）

   2）为了保证liveness, 除了上一条之外，当Proposal.highQC高于本地lockQC中的view_number时也会接收该proposal

### PRECOMMIT阶段

主节点： 

​	主节点收到quorum个当前提案的prepare-vote消息时，通过聚合quorum个部分签名得到prepareQC；然后主节点广播pre-commit消息附带聚合得到的prepareQC。

从节点： 

​	其他节点收到pre-commit消息，验证之后，发送pre-commit vote消息给主节点

​	在主节点发送的pre-commit中的prepareQC就表明了prepare消息中的提案消息，所有节点投票成功达成了共识，这一时刻与PBFT中PREPARE阶段达成共识类似。

### COMMIT阶段

​	主节点： 

   1）主节点先收集quorum个pre-commit vote消息，然后聚合出这一阶段的pre-commitQC，附带在commit消息中发送给其他节点。

   2）设置本地lockedQC为pre-commitQC。

​	从节点： 

   收到commit消息时，消息验证通过同样更新本地的lockedQC为commit消息中的pre-commitQC，对其签名并生成commit vote并发送给主节点。

​	主节点发送的commit消息附带的pre-commitQC即与PBFT中的第二轮COMMIT阶段共识类似，其中PBFT中该阶段共识表明了节点对的第一阶段达成共识这件事的共识，即确保了至少quorum个节点已经完成PREPARE阶段，在发生视图切换时，有足够多的节点能够证明对该提案达成了PREPARE共识，在新视图中提案内容需要被提交。

### DECIDE阶段

​	主节点： 

   1）收集到quorum个commit vote消息时，聚合得到commitQC，并且附带在decide消息中发送给其他节点；

   2）当其他节点收到decide消息时，其中commitQC指向的提案中的交易就会被执行；

   3）之后增加视图号viewnumber，开启下一轮共识，根据prepareQC构造New-View消息。

​	从节点： 

   验证消息后执行decide消息中commitQC指向的树节点的交易。

   在共识中任何其他阶段发生了超时事件，发送新视图的new-view消息，都会直接开启下一轮新的共识。

## **Chained** HotStuff

​	在basic hotStuff中，三阶段投票每一阶段无非都是发出消息然后收集投票。

​	在Prepare阶段的投票由当前view对应的leader1收集，集齐后生成prepareQC。然后将prepareQC发送到下一个view的leader2那里，leader2基于prepareQC开始新的prepare阶段，这是leader2的prepare阶段，同时也是leader1的precommit阶段。以此类推，leader2产生新的prepareQC,然后发送给下一个view的leader3，leader3开始自己的prepare阶段，同时也是leader1的commit阶段、leader2的precommit阶段。

​	Leader节点：

​	1）等待NewView消息，然后发出Proposal

​	2）发出Proposal后，等待其他节点的投票

​	3）向下一个Leader发出NewView消息

​	非Leader节点 

​	1）等待来自Leader的Proposal消息

​	2）收到Leader的Proposal消息后，检查消息中的QC，更新本地的prepareQC、lockedQC等变量，发出投票

​	3）向下一Leader发出NewView消息

![image-20231206151658378](.\image\image-20231206151658378.png)

### Dummy block

​	正常情况下，每个View中都有一个区块产生并集齐签名，但是情况不会总是这么完美，有时不会有新的区块产生。为了保持区块高度与viewNumber的一致，hotStuff中引入了Dummy block的概念。假如在一个View中，不能达成共识，那么就在为该View添加一个Dummy block。

![image-20231206151741562](.\image\image-20231206151741562.png)

### k-chain

​	一个区块中的QC是对其直接父区块的确认，那么我们称之为1-chain。同理，一个区块b后面没有Dummy block的情况下，连续产生了k个区块，则称这段区块链分支是对区块b的k-chain。

​	每当一个新的区块形成，节点都会检查是否会形成1-chain，2-chian，3-chain.

​	1）1-chain: 有新的prepareQC形成，更新本地的prepareQC

​	2）2-chain: 有新的precommitQC形成，更新本地的lockedQC

​	3）3-chian: 有新的commitQC形成，有新的区块分支进入commit状态，执行确认的区块分支

## **Event-Driven** HotStuff

​	Event-driven Hotstuff与其他版本原理还是核心的三阶段共识，区别只是工程实现上的便利**。从Chained HotStuff到Event-driven HotStuff，实现原论文中将整个协议的**安全性（safety）和活性（liveness）解耦。

​	Safety 通过消息进行状态处理

![image-20231206151944837](.\image\image-20231206151944837.png)

​	Liveness

​	1）选择、校验每个视图的主节点。

​	2）帮助主节点生成提案

​	![image-20231206152027687](.\image\image-20231206152027687.png)

## 思考

### Hotstuff三轮投票的意义

​	PBFT中为了保证系统在遇到恶意节点时能继续工作（活性），需要进行视图切换，而新视图中为了确定上一个视图中的区块是否达成共识，需要在view-change消息中附带自己收集到的quorum个prepare消息和相对应的一个pre-prepare消息作为证明，然后每个节点广播view-change消息请求视图切换，此时广播的消息复杂度o(n^2)，消息的量级为o(n)，因此视图切换的复杂度为o(n^3)。安全性和活性让PBFT需要o(n^3)的通信复杂度，对于网络的负载极大，限制了其向大规模网络的扩展。

​	HotStuff中如果我们对某一区块达成了两轮共识，在更换主节点时便能确定，主节点只需要基于最新的两轮共识节点产生新节点是安全的。换句话说，只需要根据区块自身的状态（经过几轮共识）就可以确定是否在新的视图中基于该区块打包块新区块，降低了在视图切换时候的通信复杂度。

### HotStuff为什么不需要像Tendermint等待Δ时间，以获取最新的prepareQC

1）HotStuff增加了一个轮次。其中第一个轮次后的QC称为prepareQC，第二个轮次后的QC称为lockedQC。Leader在发起新的proposal时将基于收集到的最高的prepareQC进行，而replica投票时将基于proposal中携带的最高prepareQC和本地保存的lockedQC作比较。

2）对于任意一个lockedQC而言，其至少在2f+1个节点中保存有对应的prepareQC，这进一步保证了leader在发起新的 proposal时至少收集到一个该prepareQC。因此，对于所有节点中保存的最高的lockedQC，leader一定会接收到其对应的 prepareQC。收到的prepareQC均会以此为基准进行比较，保证prepareQC一定大于等于lockedQC对应的prepareQC，也一定会被所有诚实节点投票。

总结来说，HotStuff通过增加一个阶段，保证了leader一定可以搜集到最高lockedQC对应的prepareQC

### HotStuff算法的通信复杂度

1）在HotStuff算法中，只有leader节点与各个replica节点之间的通信，replica节点之间不需要广播，且通信信息的大小为O(1)。

2）视图切换时需要告知下一轮主节点发生了view-change，而这一过程只需要一轮的new-view就可以达成共识。

​	因此，HotStuff算法的通信复杂度为O(n)