
set(APP_UNITTEST xbft-unittests)
set(APP_UNITTEST_SRC
    main.cpp
)

set(INNER_LIBS xbft)

# Set unittest module dir
aux_source_directory(${UNITTEST_SRC_DIR}/utils UNITTEST_UTILS_SRC)
aux_source_directory(${UNITTEST_SRC_DIR}/crypto UNITTEST_CRYPTO_SRC)
aux_source_directory(${UNITTEST_SRC_DIR}/engine UNITTEST_ENGINE_SRC)

# Generate executable files
add_executable(${APP_UNITTEST} ${UNITTEST_UTILS_SRC} ${UNITTEST_CRYPTO_SRC} ${UNITTEST_ENGINE_SRC} ${APP_UNITTEST_SRC})

target_link_libraries(${APP_UNITTEST} ${INNER_LIBS} ${XBFT_DEPENDS_LIBS} ${XBFT_DEPEND_LIB})

#disable_clang_tidy(${APP_UNITTEST})