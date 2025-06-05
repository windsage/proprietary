set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC -O3")
add_library (qcrilNrLogger-headers INTERFACE)

target_include_directories ( qcrilNrLogger-headers
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/qtiril-common/logger>
    )

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/qcril-common/logger
)

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(diag)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqcrilNrLogger
set(TARGET_LIBQCRIL_LOGGER qcrilNrLogger)

# Library source files
file(GLOB qcrilNrLogger_src "DiagLogger.cpp" "Logger.cpp")

# Build libqtimutex shared library
add_library (${TARGET_LIBQCRIL_LOGGER} SHARED
    "${qcrilNrLogger_src}"
)

set_target_properties(${TARGET_LIBQCRIL_LOGGER} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qcrilNrLogger_hdrs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/mocks
)

#target_include_directories(${TARGET_LIBQCRIL_LOGGER} PUBLIC ${qcrilNrLogger_hdrs})

add_definitions(-DTAG=\"libqcrilNrLogger\")

# Additional LDFLAGS.
set(LDFLAGS "${LDFLAGS} --no-allow-shlib-undefined,--unresolved-symbols=report-all")

target_link_libraries(${TARGET_LIBQCRIL_LOGGER}
    PRIVATE
    pthread
    PUBLIC
    diag
    mockqcrilNrcutils
    qcrilNrLogger-headers
    )

install ( TARGETS ${TARGET_LIBQCRIL_LOGGER} qcrilNrLogger-headers  EXPORT qcrilNrLoggerTargets
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set(qcrilNrLogger_VERSION 1.0.0)
include (CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/qcrilNrLogger/qcrilNrLoggerConfigVersion.cmake"
    VERSION ${qcrilNrLogger_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(EXPORT qcrilNrLoggerTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/qcrilNrLogger/qcrilNrLogger.cmake"
    NAMESPACE QtiRil::
    )

configure_file(cmake/qcrilNrLoggerConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/qcrilNrLogger/qcrilNrLoggerConfig.cmake"
    COPYONLY
    )

set(ConfigPackageLocation lib/cmake/qcrilNrLogger)
install(EXPORT qcrilNrLoggerTargets
    FILE qcrilNrLoggerTargets.cmake
    NAMESPACE QtiRil::
    DESTINATION
    ${ConfigPackageLocation}
    )

install(
    FILES
    cmake/qcrilNrLoggerConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/qcrilNrLogger/qcrilNrLoggerConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel
    )

