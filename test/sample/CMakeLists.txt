set(APP_SAMPLE xbft-sample)
set(APP_SAMPLE_SRC
    main.cpp
)

set(INNER_LIBS xbft yaml-cpp)
set(XBFT_SAMPLE_INCLUDE_DIR  
    ${SAMPLE_SRC_DIR}
    ${SAMPLE_SRC_DIR}/common
    ${SAMPLE_SRC_DIR}/dealing
    ${SAMPLE_SRC_DIR}/net
    ${SAMPLE_SRC_DIR}/proto
    )

# Set unittest module dir
aux_source_directory(${SAMPLE_SRC_DIR}/common SAMPLE_COMMON_SRC)
aux_source_directory(${SAMPLE_SRC_DIR}/dealing SAMPLE_DEALING_SRC)
aux_source_directory(${SAMPLE_SRC_DIR}/net SAMPLE_NET_SRC)
aux_source_directory(${SAMPLE_SRC_DIR}/proto SAMPLE_PROTO_SRC)

# Generate executable files
add_executable(${APP_SAMPLE} ${SAMPLE_COMMON_SRC} ${SAMPLE_DEALING_SRC} ${SAMPLE_NET_SRC} ${SAMPLE_PROTO_SRC} ${APP_SAMPLE_SRC})
target_include_directories(${APP_SAMPLE} PUBLIC ${XBFT_SAMPLE_INCLUDE_DIR})
target_link_libraries(${APP_SAMPLE} ${INNER_LIBS} ${XBFT_DEPENDS_LIBS} ${XBFT_DEPEND_LIB})  
