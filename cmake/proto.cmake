if(NOT PROTO_BIN)
    find_program(PROTO_BIN protoc ${BIFCORE_3RD_LIB}/bin)
endif()

if(NOT PROTO_BIN)
    message(FATAL_ERROR "protoc is not installed. Aborting...")
else()
    message(STATUS "protoc has been found: ${PROTO_BIN}")
endif()



file(GLOB_RECURSE PROTOS ${CMAKE_SOURCE_DIR}/src/protos/*.proto)

set(PROTO_SRC "")
set(PROTO_HDRS "")

foreach(proto ${PROTOS})
        get_filename_component(PROTO_FILE ${proto} NAME_WE)

        list(APPEND PROTO_SRC "${CMAKE_SOURCE_DIR}/src/protos/${PROTO_FILE}.pb.cc")

        list(APPEND PROTO_HDRS "${CMAKE_SOURCE_DIR}/src/protos/${PROTO_FILE}.pb.h")

        add_custom_command(
          OUTPUT "${CMAKE_SOURCE_DIR}/src/protos/${PROTO_FILE}.pb.cc"
                 "${CMAKE_SOURCE_DIR}/src/protos/${PROTO_FILE}.pb.h"
          COMMAND  ${PROTO_BIN}  --cpp_out  ${CMAKE_SOURCE_DIR}/src/protos/ -I${CMAKE_SOURCE_DIR}/src/protos/  ${proto}
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

