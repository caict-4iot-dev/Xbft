message(STATUS "start checking 3rd...")

set(XBFT_3RD_INC_DIR ${XBFT_3RD_LIB}/include)
set(XBFT_3RD_LIB_DIR ${XBFT_3RD_LIB}/lib)
set(XBFT_3RD_LIB64_DIR ${XBFT_3RD_LIB}/lib64)

if(NOT EXISTS ${XBFT_3RD_LIB})
    message(STATUS "${XBFT_3RD_LIB} not exist...")
    file(MAKE_DIRECTORY ${XBFT_3RD_LIB})
    message(STATUS "mkdir ${XBFT_3RD_LIB}...")
endif()

#check yaml-cpp
set(YAML_SRC_DIR ${XBFT_3RD_SRC_DIR}/yaml-cpp)
if(NOT EXISTS ${YAML_SRC_DIR})
    message(FATAL_ERROR "${YAML_SRC_DIR} not exist.")
endif()
message(STATUS "compile libyaml-cpp.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${YAML_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${XBFT_3RD_LIB} -DYAML_CPP_BUILD_TESTS=OFF
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
set(GTEST_SRC_DIR ${XBFT_3RD_SRC_DIR}/googletest)
if(NOT EXISTS ${GTEST_SRC_DIR})
    message(FATAL_ERROR "${GTEST_SRC_DIR} not exist.")
endif()
message(STATUS "compile libgtest.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GTEST_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${XBFT_3RD_LIB}
                WORKING_DIRECTORY "${GTEST_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GTEST_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GTEST_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)

#check openssl
set(SSL_SRC_DIR ${XBFT_3RD_SRC_DIR}/openssl)
if(NOT EXISTS ${SSL_SRC_DIR})
    message(FATAL_ERROR "${SSL_SRC_DIR} not exist.")
endif()
message(STATUS "compile libssl.a...")
execute_process(COMMAND ./config no-shared --prefix=${XBFT_3RD_LIB} --openssldir=ssl
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
set(ED25519_SRC_DIR ${XBFT_3RD_SRC_DIR}/ed25519-donna)
if(NOT EXISTS ${ED25519_SRC_DIR})
    message(FATAL_ERROR "${ED25519_SRC_DIR} not exist.")
endif()
message(STATUS "compile libed25519.a...")
execute_process(COMMAND gcc ed25519.c -m64 -O3 -c -I${XBFT_3RD_INC_DIR}
                WORKING_DIRECTORY "${ED25519_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ar cr libed25519.a ed25519.o
                WORKING_DIRECTORY "${ED25519_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
if(NOT EXISTS ${XBFT_3RD_INC_DIR}/ed25519)
    message(STATUS "${XBFT_3RD_INC_DIR}/ed25519 not exist...")
    file(MAKE_DIRECTORY ${XBFT_3RD_INC_DIR}/ed25519)
    message(STATUS "mkdir ${XBFT_3RD_INC_DIR}/ed25519...")
endif()
file(COPY ${ED25519_SRC_DIR}/libed25519.a DESTINATION ${XBFT_3RD_LIB64_DIR})
file(GLOB ed25519HeaderFiles "${ED25519_SRC_DIR}/*.h")
file(COPY ${ed25519HeaderFiles} DESTINATION ${XBFT_3RD_INC_DIR}/ed25519)

#check protobuf
set(PROTOBUF_SRC_DIR ${XBFT_3RD_SRC_DIR}/protobuf)
if(NOT EXISTS ${PROTOBUF_SRC_DIR})
    message(FATAL_ERROR "${PROTOBUF_SRC_DIR} not exist.")
endif()
message(STATUS "compile libprotobuf.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${PROTOBUF_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${XBFT_3RD_LIB} -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON 
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
set(GLOG_SRC_DIR ${XBFT_3RD_SRC_DIR}/glog)
if(NOT EXISTS ${GLOG_SRC_DIR})
    message(FATAL_ERROR "${GLOG_SRC_DIR} not exist.")
endif()
message(STATUS "compile libglog.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GLOG_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${XBFT_3RD_LIB} -DBUILD_SHARED_LIBS=OFF
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

#check gflags
set(GFLAGS_SRC_DIR ${XBFT_3RD_SRC_DIR}/gflags)
if(NOT EXISTS ${GFLAGS_SRC_DIR})
    message(FATAL_ERROR "${GFLAGS_SRC_DIR} not exist.")
endif()
message(STATUS "compile libgflags.a...")
execute_process(COMMAND ${CMAKE_COMMAND} -B ${GFLAGS_SRC_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${XBFT_3RD_LIB}
                WORKING_DIRECTORY "${GFLAGS_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${GFLAGS_SRC_DIR}/build -j8 --target install
                WORKING_DIRECTORY "${GFLAGS_SRC_DIR}"
                OUTPUT_QUIET
                COMMAND_ERROR_IS_FATAL ANY
)