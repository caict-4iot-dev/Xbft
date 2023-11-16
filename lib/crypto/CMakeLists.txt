cmake_minimum_required(VERSION 3.22)

set(LIB_XBFT_CRYPTO xbft_crypto)

aux_source_directory(${XBFT_SRC_DIR}/crypto/ CRYPTO_SRC)

add_library(${LIB_XBFT_CRYPTO} ${CRYPTO_SRC})
