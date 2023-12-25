# c++编码规范

## 1. 简介

### 1.1 目的

本文档规定了采用C++开发者采用现代 C++（ C++17）编程语言编码开发软件的规范，现有的或从其他平台移植代码并不需要根据本规范进行修改。开发人员和代码管理人员有责任使其团队编写的代码符合本文档的规定。

本文档的目的是通过定义规范和指南，促进更加统一的代码和减少常见的表达错误，但又不影响的软件工程师的创造力。

本规范的目标是：

- 通过规定统一的代码编码风格，提高软件源代码可读性；
- 提高软件源代码的可移植性；
- 提高软件源代码的可维护性；
- 为保证软件的质量，提供一个标准。

本文档并不涉及到教授C++语言。

### 1.2 适用范围

本规范适用于区块链研发团队C++语言类代码的代码编写标准。适用于代码编写阶段和代码维护阶段。



## 2. C++编码规范和指南

以下各节列出的编程规范和准则，适用于C++代码。

规范以粗体显示；指南显示为粗体、斜体。下面的每一个规范或指南，包括了：支持的理由、进一步的说明、示例和该规范明确允许的例外。

### 2.1 总则

本节涵盖了基础的编码规范和指南。

**Rule-1 每次违背规范或指南，应明确记录。**

如果在编码中需要不遵守该规范、指南，在写注释的同时，必须明确说明不遵守的规范或指南的理由。必须通过同行的审查，必须有足够的理由来支持违背规范和指南的这一决定。

**Rule-2 编译器的警告信息，不得忽略。**

许多程序员经常忽略编译器的警告信息。在C++，编译器的编写者比你对要做什么有更好地把握。不能忽视警告。

**Rule-3 当修改代码或添加一个新的文档到已有的代码库中，允许使用与已有代码相同的编码规范。**

为一致起见，按照该系统已经定义和使用的规范添加新的文档存放到已有的子系统中。

### 2.2 命名约束

本节介绍了C++标识符的命名规范。通过命名规定促进标识符定义规范，来保证开发环境的一致性。

#### 2.2.1 命名约定综述

| 名称                           | 子名称                   | 标识                                  | 示例                                  | 说明        |
| :----------------------------- | ------------------------ | ------------------------------------- | ------------------------------------- | ----------- |
| 文件                           |                          |                                       | LedgerManager.h、LedgerManager.cpp    | 大驼峰      |
| 命名空间                       |                          |                                       | bifcore                               | 全小写      |
| 类                             |                          |                                       | LedgerManager                         | 大驼峰      |
|                                | 普通成员变量             | m_                                    | m_ledgerEntity                        | 标识+小驼峰 |
|                                | 成员变量为指针           | mp_                                   | mp_ledgerEntity                       | 标识+小驼峰 |
|                                | 成员变量为引用           | mr_                                   | mr_ledgerEntity                       | 标识+小驼峰 |
|                                | 成员变量为静态变量       | ms_                                   | ms_ledgerCount                        | 标识+小驼峰 |
|                                | 类public 方法            |                                       | GetLedger                             | 大驼峰      |
|                                | 类private、protected方法 |                                       | getErrorCode                          | 小驼峰      |
| 枚举                           |                          |                                       |                                       | 大驼峰      |
|                                | 成员                     |                                       |                                       | 大驼峰      |
| 普通函数                       |                          |                                       | GetRandom                             | 大驼峰      |
| 函数参数、局部变量、结构体成员 | 指针                     | p_                                    | Transmitter*        p_transmitterBand | 标识+小驼峰 |
|                                | p_                       | shared_ptr<>        p_transmitterBand | 标识+小驼峰                           |
|                                | 引用                     | r_                                    | Tranmitter&        r_transmitterBand  | 标识+小驼峰 |
|                                | 静态                     | s_                                    | static int s_someNumber               | 标识+小驼峰 |
|                                | 常量                     | c_                                    | const int c_someName                  | 标识+小驼峰 |
|                                | 常量指针                 | cp_                                   | const Receiver&       cr_receiverFreq | 标识+小驼峰 |
|                                | 常量引用                 | cr_                                   | const Receiver*       cp_receiverFreq | 标识+小驼峰 |
| proto结构                      | message                  |                                       | LedgerEntity                          | 大驼峰      |
|                                | enum                     |                                       | MESSAGE_TYPE                          | 大写+下划线 |
|                                | 普通变量                 |                                       | receiverFreq                          | 小驼峰      |
| 全局变量                       |                          | g_                                    | g_exitFlag                            | 标识+小驼峰 |
| 常量                           |                          |                                       | ERR_CODE                              | 大写+下划线 |

###  2.3 文件

#### 2.3.1 包含文件

**Rule-1 每个被包含的文件中应包含一种机制，防止该文件被多次引用。**

同一文件多次包含将导致编译错误。

一种方法，通过使用＃ifndef/＃define在文档的开头和＃endif在文档的末尾块。以避免文件多次被引用；另一种方法做的是使用的＃pragma once指令（如编译器支持的话）。第一个方法是首选，因为它与编译器无关。

```c++
Example 

Technique for preventing multiple inclusion of an include file  

#ifndef __FOO_H__

#define __FOO_H__

// The rest of the file

#endif//foo_h__
```

**Rule-2 如果一个类只是被引用（&）或有个指向（*）该类的指针，不要包含这个类所在的头文件，而是采用声明该类替代。**

只要可以，采用类的声明而不是类定义。

```c++
Example  
// Header file of the SampleClass
// Forward Class Declarations
class ReferenceClass;             // NOT #include “ReferenceClass.h” 
class SampleClass
{
Public:
    void copy(ReferenceClass* p_reference);
    // ...
};  

// Source file of the SampleClass 
// To be able to access ReferenceClass members or instances, ReferenceClass.h 
// must be included in the source file.  
#include "ReferenceClass.h"  
void           
SampleClass::copy(ReferenceClass* p_reference)
{
    // ...
}
```

**Rule-3 UNIX的相对路径名，永远不得在＃include指令中指定。**

真正的便携式代码是独立于底层操作系统。由于这个原因，相对UNIX的搜索路径应避免包括文件的相对路径。这种搜索路径处理依赖于编译器和UNIX，不应视为理所当然的。相反，搜索路径应提供在‘make’文件内作为编译器的选项。

#### 2.3.2 注释

C++头文件、源文件模板的文件风格一般来说包括版权信息、修订历史、代码批注等。

**Rule-1 文件开头注释需要满足apeche2.0规范。**

```c++
Examle
/*!
*© COPYRIGHT 2022 Corporation CAICT All rights reserved.
 * http://www.caict.ac.cn
 * https://bitfactory.cn

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 * @brief:
 * @file:   Application.h
 * @author: zhangbo3@caict.ac.cn
 * @date:  2022/03/03  14:16:22
 */
```

**Rule-2 C++的注解应使用“//”样式。**

**Rule-3 不能把代码部分注释掉。相反，该代码必须在发布之前或在代码加入任何软件的基线内之前显式删除。**

**Rule-4 注释使用英文，保证语法正确。**

### 2.4 常量和变量

本节介绍常量，变量和它们的初始化的规范和指南。

**Rule-1 定义常量应使用const或枚举类型代替＃define。**

预处理器在处理源代码时会进行宏文本替换，这会产生消极后果。例如，如果一个常量用＃define定义，常量的名称许多调试器不支持。如果这个常数用表达式来表示，这个表达式在不同的实例化时可能取值不同，这取决于该名称的范围。此外，宏经常书写错误。

**Rule-2 避免在代码中使用数值，使用符号来代替。**

代码中的数值（“魔术数字”）应持怀疑态度来对待。如果要改变一个值，就会非常困难。大量的代码依赖于这样的值永远不会改变。该值可能存在于多处，完全找到他们也很困难。

从可移植性的角度来看，绝对值可能是产生更复杂问题的原因，数值类型的值取决于执行的过程。一般情况下，一个数值类型定义为可以包含该值的最小的类型。

**Rule-3 声明的变量应该有最小的生存范围。**

变量应该具有可能小的范围来提高代码的可读性，来避免变量不必要的分配。

**Rule-4 尽所有可能避免使用全局变量。**

静态数据可能会导致几个问题。并行多线程可能导致代码行为不可预测。因为他们有静态数据，函数不再满足可重入，其行为将不可预测。

**Rule-5 避免char*赋值。**

不再允许字符串字面值常量赋值给一个char *。如果需要用字符串字面值常量赋值和初始化一个char *，应该使用const char * 或者auto。

char *str = "hello world!"; // 将出现弃用警告。

**Rule-6 字符串赋值使用原始字面量代替字面量。**

**Rule-7 使用constexpr代替const。**

### 2.5 类

本节介绍有关类的规范和指南。

~~**Rule-1 只要有可能，类属性的初始化要使用初始化列表，而不是在构造体内初始化。**~~

**Rule-2 类的public，protected，和private部分应按顺序列出。**

**Rule-3 类成员应声明为private，除静态常量数据外。**

**Rule-4 如果一个类有虚函数，虚析构函数要提供。**

**Rule-5 显示定义所有类型构造函数。**

**Rule-6 使用override显示声明重写。**

**Rule-7 使用final显示声明不允许继承。**

### 2.6 函数

**Rule-1不要使用宏功能预处理指令＃define来获得更高效的代码，而是使用内联函数。**

**Rule-2 成员函数不改变成员变量的值（它的实例变量）应声明为const。**

**Rule-3 经常使用常引用（const＆），而不是值传递，除非使用原始数据类型或指针。**

**Rule-4公共成员函数不得返回一个非const引用或指向成员变量的指针。**

允许用户直接访问对象的私有成员数据，这些数据可能会改变，这不符合类设计者的意图。这可能导致在设计者的代码安全减弱：这种情况必须避免。

更坏的情况是指针指向将要删除的内存空间。

**Rule-5 公共成员函数不得返回引用或指针给局部变量。**

如果一个函数返回一个引用或一个的指针给局部变量，这个引用或指针使用时，它所指向的内存将已经被撤销。编译器可能会也可能不会给出警告。

**Rule-6 一个函数只有一个出口点，即一个“return”语句。**

例外：为了减少函数的复杂性，仅当代码检查期间支持多重return语句。

**Rule-7 推荐使用Lambda代替函数。**

```
[ capture ] ( params ) opt -> ret { body; }
其中 capture 是捕获列表，params 是参数表，opt 是函数选项，ret 是返回值类型，body是函数体。
```

lambda 表达式还可以通过捕获列表捕获一定范围内的变量：

- [] 不捕获任何变量。
- [&] 捕获外部作用域中所有变量，并作为引用在函数体中使用（按引用捕获）。
- [=] 捕获外部作用域中所有变量，并作为副本在函数体中使用（按值捕获）。
- [=，&foo] 按值捕获外部作用域中所有变量，并按引用捕获 foo 变量。
- [bar] 按值捕获 bar 变量，同时不捕获其他变量。
- [this] 捕获全局变量和当前类中的 this 指针，让 lambda 表达式拥有和当前类成员函数同样的访问权限。如果已经使用了 & 或者 =，就默认添加此选项。捕获 this 的目的是可以在 lamda 中使用当前类的成员函数和成员变量。

在单线程环境下，推荐使用引用捕获，在多线程异步环境下禁止使用引用捕获。

~~### 2.6 错误处理~~

~~**Rule-1 使用noexcept代替try catch。**~~

### 2.7 命名空间

任何可以声明在全局范围的对象也可以在命名空间声明。这包括类，结构，函数，变量，对象，类型定义等，在一个空间的对象并不影响其行为，但是可以防止不同命名空间名称冲突。

**Guide-1 为了避免全局性的名称带来的名称冲突，使用命名空间。**

```c++
Example
namespcace xbft{
...
}//namespace xbft
```

**Guide-2 避免在全局使用using namespace，而使用namespace::的方式替代**

### 2.8 语法

 **Rule-1 空指针赋值。**

空指针使用nullptr赋值，代替NULL。

**Rule-2 推荐使用auto。**

复杂类型如各种map的迭代器使用auto自动推导。

**Rule-3 使用区间for迭代代替for循环。**

```
std::vector<int> vec = {1, 2, 3, 4};
if (auto itr = std::find(vec.begin(), vec.end(), 3); itr != vec.end()) *itr = 4;
for (auto& element : vec)
std::cout << element << std::endl; // read only
```

**使用 make_shared() 创建 shared_ptr， make_unique() 创建 unique_ptr。**

make_shared 为构造提供了更精炼的语句。 它也提供了一个机会，通过把 shared_ptr 的使用计数和对象相邻放置，来消除为引用计数进行独立的内存分配操作。优先采用 unique_ptr 而不是 shared_ptr，除非需要共享所有权理由。unique_ptr 概念上要更简单且更可预测（知道它何时会销毁），而且更快（不需要暗中维护引用计数）。

### 2.9 其他

**Rule-1 优先使用无序容器。**

**Rule-2 优先使用智能指针。**

**Rule-3 使用C++11推荐的时间库。**

**Rule-4 推荐使用右值引用减少拷贝。**

**Rule-5 优先使用C++11推荐的线程、锁、原子操作函数。**

**Rule-6 统一使用parallel库提供的并行map等并行容器。**

**Rule-7 map中插入数据时禁止直接使用insert。**

**Rule-8 除原始数据类型，禁止使用memset或者memcpy。**

memset，memcpy，memmove，以及 memcmp 与此相似。

**Rule-9 优先使用 using 而不是 typedef 来定义别名。**

**Rule-10 禁止使用c风格的强制类型转换。**

```c++
 D* pd1 = (D*)pb;                    // 不推荐
 D* pd2 = static_cast<D*>(pb);       // 推荐
```

## 3 Review报告

本节介绍了C++标识符的命名规范。通过命名规定促进标识符定义规范，来保证开发环境的一致性。

## 参考文献

本规范参考了《C++核心编码规范》、《谷歌c++编码规范》等，并结合我们自己的实际情况做了一定的调整。