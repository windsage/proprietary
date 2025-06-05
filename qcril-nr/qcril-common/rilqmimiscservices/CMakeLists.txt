include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/services/
)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)

# Set target library name as librilqmimiscservices
set(TARGET_LIBRILQMIMISCSERVICES rilqmimiscservices)

# Library source files
file(GLOB librilqmimiscservices_src "src/*.c")

# Build librilqmimiscservices shared library
add_library (${TARGET_LIBRILQMIMISCSERVICES} SHARED
    "${librilqmimiscservices_src}"
)

set_target_properties(${TARGET_LIBRILQMIMISCSERVICES} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(librilqmimiscservices_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/
)

#target_include_directories(${TARGET_LIBRILQMIMISCSERVICES} PUBLIC ${librilqmimiscservices_headers})

target_link_libraries(${TARGET_LIBRILQMIMISCSERVICES} qmi_common_so)

install ( TARGETS ${TARGET_LIBRILQMIMISCSERVICES}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
