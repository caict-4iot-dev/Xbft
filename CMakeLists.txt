cmake_minimum_required(VERSION 3.22)

if(CLANG)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
elseif()
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
endif()

project(xbft)
set(PROJECT_VERSION 1.0.0)

if(NOT DEFINED COMPILE_3RD)
    option(COMPILE_3RD "Compile 3rd" ON)
endif()

message(STATUS "COMPILE_3RD=" ${COMPILE_3RD})

if(COMPILE_3RD)
    set(XBFT_3RD_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
    if(NOT EXISTS ${XBFT_3RD_SRC_DIR})
        message(FATAL_ERROR "Git submodules not initialized, execute:\n  git submodule update --init")
    endif()
endif()

set(XBFT_3RD_LIB ${CMAKE_CURRENT_SOURCE_DIR}/xbft-3rd)

set(XBFT_DEPEND_LIB "-lpthread -ldl -lz -lstdc++fs")

set(XBFT_DEPENDS_LIBS
    gtest
    gmock
    protobuf
    ssl
    crypto
    ed25519
    glog
)

add_compile_options(-Wall -Werror -fpermissive -Woverloaded-virtual -D_GLIBCXX_USE_CXX11_ABI=0 -DASIO_STANDALONE)

# for debug or release flag
if(NOT DEFINED RELEASE)
    option(RELEASE "For Release" OFF)
endif()

if(RELEASE)
    add_compile_options(-O3)
else()
    add_compile_options(-O0 -g)
endif()

set(CODE_COVER ON)
if(NOT DEFINED CODE_COVER)
    option(CODE_COVER "Use gcov " ON)
endif()

message(STATUS CODE_COVER=${CODE_COVER})

IF(CODE_COVER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
ENDIF()

if(NOT DEFINED SANITIZER)
    option(SANITIZER "option for sannitizer tool" OFF)
endif()

if(SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fno-omit-frame-pointer")
endif()

if(NOT DEFINED CODECHECK)
    option(CODECHECK "option for codecheck tool clang-tidy" OFF)
endif()
set(CODECHECK ON)
message(STATUS "CODECHECK:${CODECHECK}")
message(STATUS "CODE_COVER:${CODE_COVER}")

if(RELEASE)
    set(CODECHECK FALSE)
    set(SANITIZER FALSE)
    message(STATUS "CODECHECK and SANITIZER forced to FALSE because Debug type is Release")
endif()

if(COMPILE_3RD)
    include(cmake/3rd.cmake)
endif()

#include(cmake/clang-tidy.cmake)

include(cmake/proto.cmake)

if(NOT EXISTS ${XBFT_3RD_LIB})
    message(FATAL_ERROR "${XBFT_3RD_LIB} not exist...")
endif()

if(NOT DEFINED GLOG_CUSTOM_PREFIX_SUPPORT)
    option(GLOG_CUSTOM_PREFIX_SUPPORT "option for GLOG_CUSTOM_PREFIX_SUPPORT" ON)
endif()

if(GLOG_CUSTOM_PREFIX_SUPPORT)
    add_definitions(-DGLOG_CUSTOM_PREFIX_SUPPORT)
    message(STATUS "-DGLOG_CUSTOM_PREFIX_SUPPORT=" ${GLOG_CUSTOM_PREFIX_SUPPORT})
endif(GLOG_CUSTOM_PREFIX_SUPPORT)

if(NOT DEFINED BUILD_UNIT_TESTS_OPT)
    option(BUILD_UNIT_TESTS_OPT "Build unit tests" ON)
endif()

if(NOT DEFINED BUILD_SAMPLE_OPT)
    option(BUILD_SAMPLE_OPT "Build sample" OFF)
endif()

# c++17
add_definitions(-std=c++1z)

set(XBFT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(XBFT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(XBFT_ROOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(XBFT_INCLUDE_DIR
    ${XBFT_3RD_LIB}/include
    ${XBFT_ROOT_DIR}/include
    ${XBFT_SRC_DIR}/protos
    ${XBFT_SRC_DIR}/utils
    ${XBFT_SRC_DIR}/crypto
    ${XBFT_SRC_DIR}/engine
)

link_directories(
    ${XBFT_3RD_LIB}/lib64
    ${XBFT_3RD_LIB}/lib
)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

if(BUILD_UNIT_TESTS_OPT)
    set(UNITTEST_SRC_DIR ${XBFT_ROOT_DIR}/test/unittest)
    add_subdirectory(${UNITTEST_SRC_DIR})
endif()

if(BUILD_SAMPLE_OPT)
    set(SAMPLE_SRC_DIR ${XBFT_ROOT_DIR}/test/sample)
    add_subdirectory(${SAMPLE_SRC_DIR})
endif()
