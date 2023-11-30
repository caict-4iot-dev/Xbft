## 目的

本文档对依赖的三方库进行统一管理，需要描述清楚使用三方库的连接、版本号、commitid、安装步骤。

三方库全部采用**静态库**的方式，统一编译到/home/xbft-3rd 目录。

## 三方库

### yaml-cpp

源码链接：[jbeder/yaml-cpp: A YAML parser and emitter in C++ (github.com)](https://github.com/jbeder/yaml-cpp)

版本号: yaml-cpp-0.7.0

commitid:0579ae3d976091d7d664aa9d2527e0d0cff25763

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/xbft-3rd -DYAML_CPP_BUILD_TESTS=OFF
make -j8
sudo make install
sudo chmod -R 755 /home/xbft-3rd/
```

### gtest

源码链接：[google/googletest: GoogleTest - Google Testing and Mocking Framework (github.com)](https://github.com/google/googletest)

版本号：release-1.8.1

commitid:2fe3bd994b3189899d93f1d5a881e725e046fdc2

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/xbft-3rd
make -j8
sudo make install
sudo chmod -R 755 /home/xbft-3rd/
```

### openssl

源码链接：[openssl/openssl: TLS/SSL and crypto library (github.com)](https://github.com/openssl/openssl)

版本号: OpenSSL_1_1_1o

commitid: ca2e0784d2c38edcefd5d68028f4d954bd8faddb

安装步骤：

```shell
./config no-shared --prefix=/home/xbft-3rd --openssldir=ssl
make -j8
sudo make install
sudo chmod -R 755 /home/xbft-3rd/
```

### gflags

源码链接：[gflags/gflags: The gflags package contains a C++ library that implements commandline flags processing. (github.com)](https://github.com/gflags/gflags)

版本号：v2.2.2

commitid: e171aa2d15ed9eb17054558e0b3a6a413bb01067

安装步骤：

```shell
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/xbft-3rd
make -j8
sudo make install
sudo chmod -R 755 /home/xbft-3rd/
```

### ed25519-donna

源码链接：[floodyberry/ed25519-donna: Implementations of a fast Elliptic-curve Digital Signature Algorithm (github.com)](https://github.com/floodyberry/ed25519-donna)

版本号：-

commitid: 8757bd4cd209cb032853ece0ce413f122eef212c

安装步骤：

```shell
gcc ed25519.c -m64 -O3 -c
ar cr libed25519.a ed25519.o
sudo cp libed25519.a /home/xbft-3rd/lib64/
sudo mkdir /home/xbft-3rd/include/ed25519
sudo cp *.h /home/xbft-3rd/include/ed25519
sudo chmod -R 755 /home/xbft-3rd/
```

### protobuf

源码链接：[protocolbuffers/protobuf: Protocol Buffers - Google's data interchange format (github.com)](https://github.com/protocolbuffers/protobuf)

版本号：v3.9.2

commitid: 52b2447247f535663ac1c292e088b4b27d2910ef

安装步骤：

```shell
mkdir build
cd build
cmake ../cmake/  -DCMAKE_INSTALL_PREFIX=/home/xbft-3rd -Dprotobuf_BUILD_TESTS=OFF
make -j8
sudo make install
sudo chmod -R 755 /home/xbft-3rd/
```

### glog

源码链接：[google/glog: C++ implementation of the Google logging module (github.com)](https://github.com/google/glog)

版本号：v0.6.0-rc2

commitid: 864ef64920fc9c7b7c4b6efacd29053245632509

安装步骤：

```shell
mkdir build 
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/home/xbft-3rd/ -DBUILD_SHARED_LIBS=OFF -DWITH_GFLAGS=OFF -DWITH_GTEST=OFF -DWITH_PKGCONFIG=OFF -DWITH_UNWIND=OFF -DWITH_GMOCK=OFF -DBUILD_TESTING=OFF -DWITH_CUSTOM_PREFIX=ON
make -j8
sudo make install
sudo chmod -R 755 /home/xbft-3rd/
```
