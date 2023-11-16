## clang-tidy

### 简介

1、clang-tidy是c++语言开发的基于AST的静态检查工具。因为它基于AST,所以要比基于正则表达式的静态检查工具更为精准，但是带来的缺点就是要比基于正则表达式的静态检查工具慢一点。clang-tidy不仅仅可以做静态检查，还可以做一些修复工作。clang-tidy是基于[LibTooling](https://link.zhihu.com/?target=https%3A//clang.llvm.org/docs/LibTooling.html)的工具。而LibTooling是一个库，这个库主要是为了基于Clang编写单独的工具。clang-tidy属于clang的extra tools。

clang-tidy 主要可以检测一下几类问题:

- 检测违反代码规范的代码模式（header guard不满足，include头文件顺序错误)；
- 找出不容易在编译时发现的代码错误（把int赋值给std::string, 变量定义在头文件）；
- 把deprecated的API替换成新的API，modernize模块典型例子，把C++03的代码自动转换成C++11的代码（for-range-loop, auto, nullptr, overriede, default）；
- 编码规范。

### clang-tidy 支持检查的类型

clang-tidy 目前可以检测200+ 个check, 具体可以通过命令 ``` clang-tidy -listchecks -checks="*" ``` 查看，具体说明见[官网](https://clang.llvm.org/extra/clang-tidy/checks/list.html)。根据check的种类可以分为以下几类（通过check的前缀即可分辨）：

| Name prefix            | Description                                                  |
| ---------------------- | ------------------------------------------------------------ |
| abseil-                | 检测abseil库相关的规则                                       |
| altera-                | 检测FPGA相关的OpenCL规则                                     |
| android-               | 检测android相关的规则                                        |
| boost-                 | 检测boost库相关的规则                                        |
| bugprone-              | 检测代码构造是否出错                                         |
| cert-                  | 检测[CERT](https://wiki.sei.cmu.edu/confluence/display/seccode/SEI+CERT+Coding+Standards)(证书安全编码指南)的代码规范 |
| clang-analyzer-        | clang编译器静态检测工具                                      |
| **concurrency-**       | 并发相关的检测                                               |
| **cppcoreguidelines-** | 检测是否违反[cpp-core-guidelines](https://github.com/isocpp/CppCoreGuidelines)(c++核心编码指南) |
| darwin-                | 检测是否违反darwin编码规范                                   |
| fuchsia-               | 检测是否违反fuchsia编码规范                                  |
| **google-**            | 检测是否违反[google code style](https://google.github.io/styleguide/cppguide.html)(谷歌编码规范) |
| hicpp-                 | 检查有关高完整性C++编码标准                                  |
| linuxkernel-           | 与Linux内核编码约定相关的检查                                |
| llvm-                  | 与llvm编译器编码约定相关的检查                               |
| llvmlibc-              | 与LLVM libc编码标准相关的检查                                |
| misc-                  | 其它一些零碎的check                                          |
| **modernize-**         | 把C++03代码转换成C++11代码，使用C++11新特性                  |
| mpi-                   | 检测MPI API问题                                              |
| objc-                  | 与object-c编码约定相关的检查                                 |
| openmp-                | 检测open_mp API问题                                          |
| **performance-**       | 检测性能相关的问题                                           |
| portability-           | 检查是否针对与可移植性相关的问题，这些问题与任何特定的编码风格无关。 |
| **readability-**       | 检测代码上相关问题，但又不明确属于任何代码规范的             |
| zircon-                | 与Zircon内核编码约定相关的检查                               |

### clang-tidy 的使用

可以通过```clang-tidy -help``` 查看命令参数详情。

clang-tidy每次只针对一个编译单元(TranslationUnit, 可简单理解成1个`.cpp`文件)进行静态分析，因此，它只能查找出一个编译单元里面的代码问题，对于那种只在跨编译单元出现的问题，就无能为力了.

简单的检查命令如下：

```shell
clang-tidy --checks='Checks' test.cpp -- -I ./include/ 
```

其中–checks=可以写‘*’，表示对所有clang-tidy检查项进行检查，上面‘Checks’是指定检查.clang-tidy文件里的检查项（.clang-tidy 里填写展开的检查规则）

#### 在cmake 中使用clang-tidy

1. 在执行cmake 命令时添加clang-tidy

   ```shell
   cmake -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks='Checks'"
   ```

2. 在CMakeList.txt 中添加

   ```shell
    set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks='Checks'")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks='Checks'")
   ```

#### clang-tidy 详细的检测规则

参考 https://blog.csdn.net/Fenplan/article/details/119755111

或者见官网

#### 例外情况

如果有些情况不得不违反规则，需要跳过clang-tidy 的检测，可以在代码所在的行尾 添加注释**NOLINT**或者在代码的上一行添加注释**NOLINTNEXTLINE**（两种注释都只一行起作用）

### clang-tidy 的安装

```shell
sudo yum install centos-release-scl
sudo yum install  llvm-toolset-7
sudo yum install llvm-toolset-7-clang-analyzer llvm-toolset-7-clang-tools-extra
scl enable llvm-toolset-7 bash
```

### 参考链接

http://manpages.ubuntu.com/manpages/focal/man1/clang-tidy-6.0.1.html

https://www.jianshu.com/p/d6e12fc51294

https://wenku.baidu.com/view/b0039b3e68d97f192279168884868762caaebb61.html

https://releases.llvm.org/13.0.1/tools/clang/tools/extra/docs/clang-tidy/index.html