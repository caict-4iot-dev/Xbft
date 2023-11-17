cmake_minimum_required(VERSION 3.22)

aux_source_directory(${XBFT_SRC_DIR}/utils/ UTILS_SRC)
aux_source_directory(${XBFT_SRC_DIR}/crypto/ CRYPTO_SRC)
aux_source_directory(${XBFT_SRC_DIR}/protos/ PROTOS_SRC)
aux_source_directory(${XBFT_SRC_DIR}/engine/ ENGINE_SRC)


add_library(xbft
    ${UTILS_SRC}
    ${CRYPTO_SRC}
    ${PROTOS_SRC}
    ${ENGINE_SRC}
)

add_dependencies(xbft generate_proto)
target_compile_features(xbft PUBLIC cxx_std_17)
target_include_directories(xbft PUBLIC ${XBFT_INCLUDE_DIR})

set_target_properties(
    xbft
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
)
    
target_link_libraries(xbft PUBLIC ${XBFT_DEPENDS_LIBS})
