cmake_minimum_required(VERSION 3.22)

set(LIB_XBFT_SAMPLE_PROTOS xbft_sample_protos)

add_library(${LIB_XBFT_SAMPLE_PROTOS} ${PROTO_SRC})

add_dependencies(${LIB_XBFT_SAMPLE_PROTOS} generate_proto)

#disable_clang_tidy(${LIB_XBFT_SAMPLE_PROTOS})