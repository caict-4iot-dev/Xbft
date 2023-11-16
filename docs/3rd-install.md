## 目的

本文档对依赖的三方库进行统一管理，需要描述清楚使用三方库的连接、版本号、commitid、安装步骤。

三方库全部采用**静态库**的方式，统一编译到/home/bif-3rd 目录。

## 三方库

### yaml-cpp

源码链接：[jbeder/yaml-cpp: A YAML parser and emitter in C++ (github.com)](https://github.com/jbeder/yaml-cpp)

版本号: yaml-cpp-0.7.0

commitid:0579ae3d976091d7d664aa9d2527e0d0cff25763

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd -DYAML_CPP_BUILD_TESTS=OFF
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### gtest

源码链接：[google/googletest: GoogleTest - Google Testing and Mocking Framework (github.com)](https://github.com/google/googletest)

版本号：release-1.8.1

commitid:2fe3bd994b3189899d93f1d5a881e725e046fdc2

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### gmssl

源码链接：[guanzhi/GmSSL: 支持国密SM2/SM3/SM4/SM9/SSL的密码工具箱 (github.com)](https://github.com/guanzhi/GmSSL)

版本号: v3.0.0

commitid:75155a4c3752ae11b35384e166122751285410a7

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### openssl

源码链接：[openssl/openssl: TLS/SSL and crypto library (github.com)](https://github.com/openssl/openssl)

版本号: OpenSSL_1_1_1o

commitid: ca2e0784d2c38edcefd5d68028f4d954bd8faddb

安装步骤：

```shell
./config no-shared --prefix=/home/bif-3rd --openssldir=ssl
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### gflags

源码链接：[gflags/gflags: The gflags package contains a C++ library that implements commandline flags processing. (github.com)](https://github.com/gflags/gflags)

版本号：v2.2.2

commitid: e171aa2d15ed9eb17054558e0b3a6a413bb01067

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### rocksdb

源码链接：[facebook/rocksdb: A library that provides an embeddable, persistent key-value store for fast storage. (github.com)](https://github.com/facebook/rocksdb)

版本号：v7.2.0

commitid: 14fb86d1badc444310d7ef6c3407732332106659

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd -DROCKSDB_BUILD_SHARED=OFF -DWITH_TESTS=OFF -DWITH_BENCHMARK_TOOLS=OFF
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/

```

### ed25519-donna

源码链接：[floodyberry/ed25519-donna: Implementations of a fast Elliptic-curve Digital Signature Algorithm (github.com)](https://github.com/floodyberry/ed25519-donna)

版本号：-

commitid: 8757bd4cd209cb032853ece0ce413f122eef212c

安装步骤：

```shell
gcc ed25519.c -m64 -O3 -c
ar cr libed25519.a ed25519.o
sudo cp libed25519.a /home/bif-3rd/lib64/
sudo mkdir /home/bif-3rd/include/ed25519
sudo cp *.h /home/bif-3rd/include/ed25519
sudo chmod -R 755 /home/bif-3rd/
```

### protobuf

源码链接：[protocolbuffers/protobuf: Protocol Buffers - Google's data interchange format (github.com)](https://github.com/protocolbuffers/protobuf)

版本号：v3.9.2

commitid: 52b2447247f535663ac1c292e088b4b27d2910ef

安装步骤：

```shell
mkdir build
cd build
cmake ../cmake/  -DCMAKE_INSTALL_PREFIX=/home/bif-3rd -Dprotobuf_BUILD_TESTS=OFF
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### glog

源码链接：[google/glog: C++ implementation of the Google logging module (github.com)](https://github.com/google/glog)

版本号：v0.6.0-rc2

commitid: 864ef64920fc9c7b7c4b6efacd29053245632509

安装步骤：

```shell
mkdir build 
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd/ -DBUILD_SHARED_LIBS=OFF -DWITH_GFLAGS=OFF -DWITH_GTEST=OFF -DWITH_PKGCONFIG=OFF -DWITH_UNWIND=OFF -DWITH_GMOCK=OFF -DBUILD_TESTING=OFF -DWITH_CUSTOM_PREFIX=ON
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### cares

源码链接：[c-ares/c-ares: A C library for asynchronous DNS requests. (github.com)](https://github.com/c-ares/c-ares)

版本号：c-ares-1_17_0

commitid: 799e81d4ace75af7d530857d4f8b35913a27463e

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd -DCARES_STATIC=ON -DCARES_STATIC_PIC=ON -DCARES_SHARED=OFF
make -j8
make install
```

### benchmark

源码链接：[google/benchmark: A microbenchmark support library. (github.com)](https://github.com/google/benchmark)

版本号：v1.7.0

commitid: 361e8d1cfe0c6c36d30b39f1b61302ece5507320

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd -DBENCHMARK_DOWNLOAD_DEPENDENCIES=OFF -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
make -j8
make install
```

### grpc

源码链接：[grpc/grpc: The C based gRPC (C++, Python, Ruby, Objective-C, PHP, C#) (github.com)](https://github.com/grpc/grpc)

版本号：v1.9.1

commitid: d45132a2e9246b11ddd0b70c07160076d5cbbb12

安装步骤：

```shell
mkdir build 
cd build

vim ../CMakeLists.txt
添加如下代码
add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)
保存

cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd -DCMAKE_BUILD_TYPE=Release -DgRPC_INSTALL=ON -DBUILD_SHARED_LIBS=OFF -DgRPC_BUILD_TESTS=OFF -DgRPC_ZLIB_PROVIDER=package -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_SSL_PROVIDER=package  -DgRPC_CARES_PROVIDER=package -DgRPC_GFLAGS_PROVIDER=package -DgRPC_BENCHMARK_PROVIDER=package -DOPENSSL_ROOT_DIR=/home/bif-3rd -DOPENSSL_LIBRARIES=/home/bif-3rd/lib64

make -j8
make install
```

### jsoncpp

源码链接：[open-source-parsers/jsoncpp: A C++ library for interacting with JSON. (github.com)](https://github.com/open-source-parsers/jsoncpp)

版本号：1.9.5

commitid: 5defb4ed1a4293b8e2bf641e16b156fb9de498cc

安装步骤：

```shell
mkdir build 
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/bif-3rd/  -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF -DJSONCPP_WITH_CMAKE_PACKAGE=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_OBJECT_LIBS=OFF 
make -j8
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### oneTBB

源码链接：[oneapi-src/oneTBB: oneAPI Threading Building Blocks (oneTBB). (github.com)](https://github.com/oneapi-src/oneTBB)

版本号：v2020.3

commitid: eca91f16d7490a8abfdee652dadf457ec820cc37

安装步骤：

```shell
make extra_inc=big_iron.inc
sudo cp -r include/* /home/bif-3rd/include
sudo cp -r build/linux_intel64_gcc_cc8.3.1_libc2.17_kernel3.10.0_release/*.a /home/bif-3rd/lib64
sudo chmod -R 755 /home/bif-3rd/
```

### CLI11
源码链接：[CLIUtils/CLI11: CLI11 is a command line parser for C++11 and beyond that provides a rich feature set with a simple and intuitive interface. (github.com)](https://github.com/CLIUtils/CLI11)

版本号：v2.2.0

commitid:b9be5b9444772324459989177108a6a65b8b2769

安装步骤:

```
mkdir build
cd build
cmake ..  -DCMAKE_INSTALL_PREFIX=/home/bif-3rd/ -DCLI11_BUILD_DOCS=OFF -DCLI11_BUILD_TESTS=OFF -DCLI11_BUILD_EXAMPLES=OFF
sudo make install
sudo chmod -R 755 /home/bif-3rd/
```

### Boost

源码链接：https://boostorg.jfrog.io/artifactory/main/release/1.70.0/source/

版本号：v1.70.0

安装步骤:

```shell
mkdir build
sudo ./bootstrap.sh  --with-libraries=thread
sudo ./b2 runtime-link=static link=static --threading=multi  --variant=release --prefix=/home/bif-3rd/ install
```

### intx

源码链接：[chfast/intx: extended precision integer library. (github.com)](https://github.com/chfast/intx)

版本号：v0.8.0

commitid:58c0c7f188a8d63c698716eb444363e3a3737a4c

安装步骤:

```shell
cp -r include/intx/ /home/bif-3rd/include/
```
