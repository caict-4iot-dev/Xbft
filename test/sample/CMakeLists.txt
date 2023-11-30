set(APP_SAMPLE xbft-sample)
set(APP_SAMPLE_SRC
    main.cpp
)

set(INNER_LIBS xbft yaml-cpp)
set(XBFT_SAMPLE_INCLUDE_DIR
    ${SAMPLE_SRC_DIR}/common
    ${SAMPLE_SRC_DIR}/dealing
    ${SAMPLE_SRC_DIR}/net
    ${SAMPLE_SRC_DIR}/protos
    )

# Set unittest module dir
aux_source_directory(${SAMPLE_SRC_DIR}/common SAMPLE_COMMON_SRC)
aux_source_directory(${SAMPLE_SRC_DIR}/dealing SAMPLE_DEALING_SRC)
aux_source_directory(${SAMPLE_SRC_DIR}/net SAMPLE_NET_SRC)
aux_source_directory(${SAMPLE_SRC_DIR}/protos SAMPLE_PROTO_SRC)

link_directories(${LIBRARY_OUTPUT_PATH})

# Generate executable files
add_executable(${APP_SAMPLE} ${SAMPLE_COMMON_SRC} ${SAMPLE_DEALING_SRC} ${SAMPLE_NET_SRC} ${SAMPLE_PROTO_SRC} ${APP_SAMPLE_SRC})
target_include_directories(${APP_SAMPLE} PUBLIC ${XBFT_SAMPLE_INCLUDE_DIR} ${XBFT_INCLUDE_DIR})
target_link_libraries(${APP_SAMPLE} ${INNER_LIBS} ${XBFT_DEPENDS_LIBS} ${XBFT_DEPEND_LIB})
add_dependencies(${APP_SAMPLE} generate_proto) 

if(BUILD_SAMPLE_OPT)
    list(APPEND NODE_LIST node0 node1 node2 node3)
    list(APPEND CONFIG_LIST config0.yaml config1.yaml config2.yaml config3.yaml)
   
    foreach(i RANGE 3)
        list(GET NODE_LIST ${i} NODE_PATH)
        install(TARGETS ${APP_SAMPLE}
            RUNTIME DESTINATION ${INSTALL_SAMPLE_PATH}/${NODE_PATH}
            )

        list(GET CONFIG_LIST ${i} CONFIG_FILE)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/${CONFIG_FILE}
            DESTINATION ${INSTALL_SAMPLE_PATH}/${NODE_PATH}
            RENAME config.yaml
            )
    endforeach()
endif()


