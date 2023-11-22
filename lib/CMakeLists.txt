cmake_minimum_required(VERSION 3.22)

aux_source_directory(${XBFT_SRC_DIR}/utils/ UTILS_SRC)
aux_source_directory(${XBFT_SRC_DIR}/crypto/ CRYPTO_SRC)
aux_source_directory(${XBFT_SRC_DIR}/protos/ PROTOS_SRC)
aux_source_directory(${XBFT_SRC_DIR}/engine/ ENGINE_SRC)

add_library(xbft_static STATIC 
    ${UTILS_SRC}
    ${CRYPTO_SRC}
    ${PROTOS_SRC}
    ${ENGINE_SRC}
)
set_target_properties(xbft_static PROPERTIES OUTPUT_NAME "xbft")
set_target_properties(xbft_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
add_library(xbft SHARED 
    ${UTILS_SRC}
    ${CRYPTO_SRC}
    ${PROTOS_SRC}
    ${ENGINE_SRC}
)
set_target_properties(xbft PROPERTIES OUTPUT_NAME "xbft")
set_target_properties(xbft PROPERTIES CLEAN_DIRECT_OUTPUT 1)

foreach(_target xbft_static xbft)
    target_include_directories(${_target} PUBLIC ${XBFT_INCLUDE_DIR})
    add_dependencies(${_target} generate_proto)
    target_compile_features(${_target} PUBLIC cxx_std_17)

    set_target_properties(
        ${_target}
        PROPERTIES
        VERSION ${PROJECT_VERSION}
        #LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
    )
    
    target_link_libraries(${_target} PUBLIC ${XBFT_DEPENDS_LIBS})
endforeach()

install(TARGETS xbft xbft_static
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )

set(CONSENGINE_H ${XBFT_ROOT_INCLUDE_DIR}/ConsEngine.h)
set(XBFT_H ${XBFT_ROOT_INCLUDE_DIR}/Xbft.h)
install(FILES ${CONSENGINE_H} ${XBFT_H} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/xbft)
