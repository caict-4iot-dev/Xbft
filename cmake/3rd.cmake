message(STATUS "start checking 3rd...")

set(BIFCORE_3RD_INC_DIR ${BIFCORE_3RD_LIB}/include)
set(BIFCORE_3RD_LIB_DIR ${BIFCORE_3RD_LIB}/lib)
set(BIFCORE_3RD_LIB64_DIR ${BIFCORE_3RD_LIB}/lib64)

if(NOT EXISTS ${BIFCORE_3RD_LIB})
    message(STATUS "${BIFCORE_3RD_LIB} not exist...")
    file(MAKE_DIRECTORY ${BIFCORE_3RD_LIB})
    message(STATUS "mkdir ${BIFCORE_3RD_LIB}...")
endif()

#check yaml-cpp
set(YAML_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/yaml-cpp)
if(NOT EXISTS ${YAML_SRC_DIR})
    message(FATAL_ERROR "${YAML_SRC_DIR} not exist.")
endif()
message(STATUS "compile libyaml-cpp.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${YAML_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB} -DYAML_CPP_BUILD_TESTS=OFF
                WORKING_DIRECTORY "${YAML_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${YAML_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${YAML_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check gtest
set(GTEST_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/googletest)
if(NOT EXISTS ${GTEST_SRC_DIR})
    message(FATAL_ERROR "${GTEST_SRC_DIR} not exist.")
endif()
message(STATUS "compile libgtest.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GTEST_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                WORKING_DIRECTORY "${GTEST_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GTEST_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GTEST_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check gmssl
set(GMSSL_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/GmSSL)
if(NOT EXISTS ${GMSSL_SRC_DIR})
    message(FATAL_ERROR "${GMSSL_SRC_DIR} not exist.")
endif()
message(STATUS "compile libgmssl.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GMSSL_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                WORKING_DIRECTORY "${GMSSL_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GMSSL_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GMSSL_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check openssl
set(SSL_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/openssl)
if(NOT EXISTS ${SSL_SRC_DIR})
    message(FATAL_ERROR "${SSL_SRC_DIR} not exist.")
endif()
message(STATUS "compile libssl.a...")
execute_process(COMMAND ./config no-shared --prefix=${BIFCORE_3RD_LIB} --openssldir=ssl
                WORKING_DIRECTORY "${SSL_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND make -j8
                WORKING_DIRECTORY "${SSL_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND make install
                WORKING_DIRECTORY "${SSL_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check ed25519
#depends openssl
set(ED25519_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/ed25519)
if(NOT EXISTS ${ED25519_SRC_DIR})
    message(FATAL_ERROR "${ED25519_SRC_DIR} not exist.")
endif()
message(STATUS "compile libed25519.a...")
execute_process(COMMAND gcc ed25519.c -m64 -O3 -c -I${BIFCORE_3RD_INC_DIR}
                WORKING_DIRECTORY "${ED25519_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ar cr libed25519.a ed25519.o
                WORKING_DIRECTORY "${ED25519_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
file(MAKE_DIRECTORY ${BIFCORE_3RD_INC_DIR}/ed25519)
file(COPY ${ED25519_SRC_DIR}/libed25519.a DESTINATION ${BIFCORE_3RD_LIB64_DIR})
file(GLOB ed25519HeaderFiles "${ED25519_SRC_DIR}/*.h")
file(COPY ${ed25519HeaderFiles} DESTINATION ${BIFCORE_3RD_INC_DIR}/ed25519)

#check protobuf
set(PROTOBUF_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/protobuf)
if(NOT EXISTS ${PROTOBUF_SRC_DIR})
    message(FATAL_ERROR "${PROTOBUF_SRC_DIR} not exist.")
endif()
message(STATUS "compile libprotobuf.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${PROTOBUF_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB} -Dprotobuf_BUILD_TESTS=OFF
                WORKING_DIRECTORY "${PROTOBUF_SRC_DIR}/cmake"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${PROTOBUF_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${PROTOBUF_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check glog
set(GLOG_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/glog)
if(NOT EXISTS ${GLOG_SRC_DIR})
    message(FATAL_ERROR "${GLOG_SRC_DIR} not exist.")
endif()
message(STATUS "compile libglog.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GLOG_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB} -DBUILD_SHARED_LIBS=OFF
                -DWITH_GFLAGS=OFF -DWITH_GTEST=OFF -DWITH_PKGCONFIG=OFF -DWITH_UNWIND=OFF
                -DWITH_GMOCK=OFF -DBUILD_TESTING=OFF -DWITH_CUSTOM_PREFIX=ON
                WORKING_DIRECTORY "${GLOG_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GLOG_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GLOG_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check jsoncpp
set(JSONCPP_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/jsoncpp)
if(NOT EXISTS ${JSONCPP_SRC_DIR})
    message(FATAL_ERROR "${JSONCPP_SRC_DIR} not exist.")
endif()
message(STATUS "compile libjsoncpp.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${JSONCPP_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB} -DJSONCPP_WITH_TESTS=OFF
                -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF
                -DJSONCPP_WITH_CMAKE_PACKAGE=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_OBJECT_LIBS=OFF
                WORKING_DIRECTORY "${JSONCPP_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${JSONCPP_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${JSONCPP_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check TBBone
set(TBB_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/oneTBB)
if(NOT EXISTS ${TBB_SRC_DIR})
    message(FATAL_ERROR "${TBB_SRC_DIR} not exist.")
endif()
message(STATUS "compile libtbb.a...")
execute_process(COMMAND make extra_inc=big_iron.inc
                WORKING_DIRECTORY "${TBB_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
file(GLOB tbbHeaderFiles "${TBB_SRC_DIR}/include/*")
file(COPY ${tbbHeaderFiles} DESTINATION ${BIFCORE_3RD_INC_DIR})
file(GLOB tbbLibFiles "${TBB_SRC_DIR}/build/linux_*/*.a")
file(COPY ${tbbLibFiles} DESTINATION ${BIFCORE_3RD_LIB64_DIR})

#check CLI11
set(CTL11_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/CLI11)
if(NOT EXISTS ${CTL11_SRC_DIR})
    message(FATAL_ERROR "${CTL11_SRC_DIR} not exist.")
endif()
message(STATUS "compile CLI11...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${CTL11_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                -DCLI11_BUILD_DOCS=OFF -DCLI11_BUILD_TESTS=OFF -DCLI11_BUILD_EXAMPLES=OFF
                WORKING_DIRECTORY "${CTL11_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND make install
                WORKING_DIRECTORY "${CTL11_SRC_DIR}/build"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check Boost
set(BOOST_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/boost)
if(NOT EXISTS ${BOOST_SRC_DIR})
    message(STATUS "${BOOST_SRC_DIR} not exist.")
    set(BOOST_SRC_FILE ${BIFCORE_3RD_SRC_DIR}/boost_1_70_0.tar.gz)
    #download
    if(NOT EXISTS ${BOOST_SRC_FILE})
        set(BOOST_SRC_URL https://boostorg.jfrog.io/artifactory/main/release/1.70.0/source/boost_1_70_0.tar.gz)
        SET(BOOST_DOWNLOAD_TIMEOUT 600 CACHE STRING "Timeout in seconds when downloading boost.")
        message(STATUS "Downloading ${BOOST_SRC_FILE}")
        file(DOWNLOAD ${BOOST_SRC_URL} ${BOOST_SRC_FILE}
             TIMEOUT ${BOOST_DOWNLOAD_TIMEOUT}
             STATUS ERR
             SHOW_PROGRESS
        )
        if(ERR EQUAL 0)
            message(STATUS "Download ${BOOST_SRC_FILE} succeed")
        else()
            file(REMOVE ${BOOST_SRC_FILE})
            file(REMOVE_RECURSE ${BOOST_SRC_DIR})
            message(STATUS "Download ${BOOST_SRC_FILE} failed, error: ${ERR}")
            message(FATAL_ERROR "You can try downloading boost files manually using curl/wget or a similar tool.\n"
                                "The url is: ${BOOST_SRC_URL}"
            )
        endif()
    endif()
    #extract
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${BOOST_SRC_FILE}
                    WORKING_DIRECTORY "${BIFCORE_3RD_SRC_DIR}"
                    OUTPUT_QUIET
                    RESULT_VARIABLE tar_result
                    COMMAND_ERROR_IS_FATAL ANY
    )
    if(tar_result MATCHES 0)
        message(STATUS "Extract ${BOOST_SRC_FILE} succeed")
        set(BOOST_OLD_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/boost_1_70_0)
        file(RENAME ${BOOST_OLD_SRC_DIR} ${BOOST_SRC_DIR})
    else()
        file(REMOVE ${BOOST_SRC_FILE})
        file(REMOVE_RECURSE ${BOOST_SRC_DIR})
        message(FATAL_ERROR "Failed to extract boost files.\n"
                        "Please try downloading and extracting yourself.\n"
                        "The url is: ${BOOST_SRC_URL}"
        )
    endif()
endif()
message(STATUS "compile libboost_thread.a...")
execute_process(COMMAND ./bootstrap.sh  --with-libraries=thread
                WORKING_DIRECTORY "${BOOST_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ./b2 runtime-link=static link=static --threading=multi  --variant=release --prefix=${BIFCORE_3RD_LIB} install
                WORKING_DIRECTORY "${BOOST_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check intx
set(INTX_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/intx)
if(NOT EXISTS ${INTX_SRC_DIR})
    message(FATAL_ERROR "${INTX_SRC_DIR} not exist.")
endif()
message(STATUS "compile intx...")
file(GLOB intxHeaderFiles "${INTX_SRC_DIR}/include/intx/*")
file(COPY "${INTX_SRC_DIR}/include/" DESTINATION ${BIFCORE_3RD_INC_DIR})

#check c-ares
set(CARES_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/cares)
if(NOT EXISTS ${CARES_SRC_DIR})
    message(FATAL_ERROR "${CARES_SRC_DIR} not exist.")
endif()
message(STATUS "compile libcares.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${CARES_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                -DCARES_STATIC=ON -DCARES_STATIC_PIC=ON -DCARES_SHARED=OFF
                WORKING_DIRECTORY "${CARES_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${CARES_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${CARES_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check benchmark
set(BENCHMARK_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/benchmark)
if(NOT EXISTS ${BENCHMARK_SRC_DIR})
    message(FATAL_ERROR "${BENCHMARK_SRC_DIR} not exist.")
endif()
message(STATUS "compile libbenchmark.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${BENCHMARK_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                -DBENCHMARK_DOWNLOAD_DEPENDENCIES=OFF -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
                WORKING_DIRECTORY "${BENCHMARK_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${BENCHMARK_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${BENCHMARK_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check gflags
set(GFLAGS_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/gflags)
if(NOT EXISTS ${GFLAGS_SRC_DIR})
    message(FATAL_ERROR "${GFLAGS_SRC_DIR} not exist.")
endif()
message(STATUS "compile libgflags.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GFLAGS_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                WORKING_DIRECTORY "${GFLAGS_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GFLAGS_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GFLAGS_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check grpc
#depends cares zlib benchmark gflags protobuf openssl
set(GRPC_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/grpc)
if(NOT EXISTS ${GRPC_SRC_DIR})
    message(FATAL_ERROR "${GRPC_SRC_DIR} not exist.")
endif()
message(STATUS "compile libgrpc.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GRPC_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB}
                -DCMAKE_BUILD_TYPE=Release -DgRPC_INSTALL=ON -DBUILD_SHARED_LIBS=OFF -DgRPC_BUILD_TESTS=OFF
                -DgRPC_ZLIB_PROVIDER=package -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_SSL_PROVIDER=package
                -DgRPC_CARES_PROVIDER=package -DgRPC_GFLAGS_PROVIDER=package -DgRPC_BENCHMARK_PROVIDER=package
                -DOPENSSL_ROOT_DIR=${BIFCORE_3RD_LIB} -DOPENSSL_LIBRARIES=${BIFCORE_3RD_LIB64_DIR}
                WORKING_DIRECTORY "${GRPC_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GRPC_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GRPC_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check rocksdb
#depends gflags
set(ROCKSDB_SRC_DIR ${BIFCORE_3RD_SRC_DIR}/rocksdb)
if(NOT EXISTS ${ROCKSDB_SRC_DIR})
    message(FATAL_ERROR "${ROCKSDB_SRC_DIR} not exist.")
endif()
message(STATUS "compile librocksdb.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${ROCKSDB_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${BIFCORE_3RD_LIB} -DROCKSDB_BUILD_SHARED=OFF -DWITH_TESTS=OFF -DWITH_BENCHMARK_TOOLS=OFF
                WORKING_DIRECTORY "${ROCKSDB_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${ROCKSDB_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${ROCKSDB_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
