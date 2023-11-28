cmake_minimum_required(VERSION 3.22)

aux_source_directory(${XBFT_SRC_DIR}/utils/ UTILS_SRC)
aux_source_directory(${XBFT_SRC_DIR}/crypto/ CRYPTO_SRC)
aux_source_directory(${XBFT_SRC_DIR}/engine/ ENGINE_SRC)

if(BUILD_STATIC_LIBS)
    add_library(xbft_static STATIC 
        ${UTILS_SRC}
        ${CRYPTO_SRC}
        ${PROTO_SRC}
        ${ENGINE_SRC}
    )
    set_target_properties(xbft_static PROPERTIES OUTPUT_NAME "xbft")
    set_target_properties(xbft_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    list(APPEND LIBS_LIST "xbft_static")
endif()

if(BUILD_SHARED_LIBS)
    add_library(xbft SHARED 
        ${UTILS_SRC}
        ${CRYPTO_SRC}
        ${PROTO_SRC}
        ${ENGINE_SRC}
    )
    set_target_properties(xbft PROPERTIES OUTPUT_NAME "xbft")
    set_target_properties(xbft PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    list(APPEND LIBS_LIST "xbft")
endif()

foreach(_target ${LIBS_LIST})
    target_include_directories(${_target} PUBLIC ${XBFT_INCLUDE_DIR} ${XBFT_SRC_DIR}/protos)
    add_dependencies(${_target} generate_proto)
    target_compile_features(${_target} PUBLIC cxx_std_17)

    set_target_properties(
        ${_target}
        PROPERTIES
        VERSION ${PROJECT_VERSION}
        LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}
    )
    
    target_link_libraries(${_target} PUBLIC ${XBFT_DEPENDS_LIBS})
endforeach()

if(BUILD_STATIC_LIBS)
    install(TARGETS xbft_static
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )
endif()

if(BUILD_SHARED_LIBS)
    install(TARGETS xbft
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )
endif()

set(CONSENGINE_H ${XBFT_ROOT_INCLUDE_DIR}/ConsEngine.h)
set(XBFT_H ${XBFT_ROOT_INCLUDE_DIR}/Xbft.h)
install(FILES ${CONSENGINE_H} ${XBFT_H} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/xbft)
