cmake_minimum_required(VERSION 3.22)

set(LIB_XBFT_ENGINE xbft_engine)

aux_source_directory(${XBFT_SRC_DIR}/engine/ ENGINE_SRC)

add_library(${LIB_XBFT_ENGINE} ${ENGINE_SRC})

add_dependencies(${LIB_XBFT_ENGINE} generate_rpc_proto generate_proto)
