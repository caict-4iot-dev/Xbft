cmake_minimum_required(VERSION 3.22)

add_subdirectory(crypto)
add_subdirectory(utils)
add_subdirectory(protos)
add_subdirectory(engine)

add_library(xbft PUBLIC)
target_compile_features(xbft PUBLIC cxx_std_17)
target_include_directories(xbft PUBLIC 
    $<BUILD_INTERFACE:${XBFT_INCLUDE_DIR}>$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set(INNER_LIBS xbft_engine xbft_crypto xbft_utils xbft_protos)


set_target_properties(
    xbft
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_SOVERSION}
)
    
target_link_libraries(xbft PUBLIC ${INNER_LIBS} ${XBFT_DEPENDS_LIBS})
