cmake_minimum_required(VERSION 3.22)

set(LIB_XBFT_UTILS xbft_utils)

aux_source_directory(${XBFT_SRC_DIR}/utils/ UTILS_SRC)

add_library(${LIB_XBFT_UTILS} ${UTILS_SRC})

