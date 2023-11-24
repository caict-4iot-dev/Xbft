if(NOT PROTO_BIN)
    find_program(PROTO_BIN protoc ${XBFT_3RD_LIB}/bin)
endif()

if(NOT PROTO_BIN)
    message(FATAL_ERROR "protoc is not installed. Aborting...")
else()
    message(STATUS "protoc has been found: ${PROTO_BIN}")
endif()



file(GLOB_RECURSE PROTOS ${CMAKE_SOURCE_DIR}/lib/protos/*.proto ${CMAKE_SOURCE_DIR}/test/sample/protos/*.proto)

set(PROTO_SRC "")
set(PROTO_HDRS "")

foreach(proto ${PROTOS})
        get_filename_component(PROTO_FILE ${proto} NAME_WE)
        get_filename_component(PROTO_PATH ${proto} DIRECTORY)

        list(APPEND PROTO_SRC "${PROTO_PATH}/${PROTO_FILE}.pb.cc")

        list(APPEND PROTO_HDRS "${PROTO_PATH}/${PROTO_FILE}.pb.h")

        add_custom_command(
          OUTPUT "${PROTO_PATH}/${PROTO_FILE}.pb.cc"
                 "${PROTO_PATH}/${PROTO_FILE}.pb.h"
          COMMAND  ${PROTO_BIN}  --cpp_out  ${PROTO_PATH} -I${PROTO_PATH}  ${proto}
          DEPENDS ${proto}
          COMMENT "Running C++ protocol buffer compiler on ${proto}"
          VERBATIM
        )
endforeach()

set_source_files_properties(${PROTO_SRC} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

add_custom_target(generate_proto ALL
                DEPENDS ${PROTO_SRC} ${PROTO_HDRS}
                COMMENT "generate proto target"
                VERBATIM
)

