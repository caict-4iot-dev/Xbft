
set(APP_UNITTEST xbft-unittests)
set(APP_UNITTEST_SRC
    main.cpp
)

set(INNER_LIBS xbft)

# Set unittest module dir
aux_source_directory(${UNITTEST_SRC_DIR}/utils UNITTEST_UTILS_SRC)
aux_source_directory(${UNITTEST_SRC_DIR}/engine UNITTEST_ENGINE_SRC)

link_directories(${LIBRARY_OUTPUT_PATH})

# Generate executable files
add_executable(${APP_UNITTEST} ${UNITTEST_UTILS_SRC} ${UNITTEST_ENGINE_SRC} ${APP_UNITTEST_SRC} ${PROTO_SRC})
target_include_directories(${APP_UNITTEST} PUBLIC ${XBFT_INCLUDE_DIR} ${XBFT_SRC_DIR}/engine ${XBFT_SRC_DIR}/protos)
target_link_libraries(${APP_UNITTEST} ${INNER_LIBS} ${XBFT_DEPENDS_LIBS} ${XBFT_DEPEND_LIB})
add_dependencies(${APP_UNITTEST} generate_proto)

#disable_clang_tidy(${APP_UNITTEST})