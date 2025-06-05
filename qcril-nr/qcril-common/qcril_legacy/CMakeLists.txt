include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc/
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/PeripheralMgr/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/utilities/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(qmi-framework)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --unresolved-symbols=report-all")

# Set target library name as libril-legacy
set(TARGET_LIBQCRIL_LEGACY ril-legacy)

# Library source files
file(GLOB libril-legacy_src "src/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_LEGACY} SHARED
    "${libril-legacy_src}"
)

set_target_properties(${TARGET_LIBQCRIL_LEGACY} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(ril-legacy_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

#target_include_directories(${TARGET_LIBQCRIL_LEGACY} PUBLIC ${ril-legacy_headers})

target_link_libraries(${TARGET_LIBQCRIL_LEGACY}
    qtimutex
    pthread
    qcrilNrLogger
    qcrilNrUtils
    qtiPeripheralMgr
    ${GLIB_LIBRARIES}
    )

install ( TARGETS ${TARGET_LIBQCRIL_LEGACY}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
