if(CODECHECK)
    if(NOT CLANG_TIDY_BIN)
        find_program(CLANG_TIDY_BIN clang-tidy)
    endif()

    if(NOT CLANG_TIDY_BIN)
        message(FATAL_ERROR "clang-tidy is not installed. Aborting...")
    else()
        message(STATUS "clang-tidy has been found: ${CLANG_TIDY_BIN}")
    endif()

    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks='Checks' -header-filter=.*")
endif()

function(disable_clang_tidy target)
    if(CODECHECK)
        set_target_properties(${target} PROPERTIES
            C_CLANG_TIDY ""
            CXX_CLANG_TIDY ""
        )
    endif()
endfunction()