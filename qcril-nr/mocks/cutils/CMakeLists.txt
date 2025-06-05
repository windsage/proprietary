set( CMAKE_CONFIGURATION_TYPES
        "Debug"
        "Release"
        )
set( CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} CACHE STRING "Available build configurations." FORCE )

SYSR_INCLUDE_DIR(qcril-nr)

include_directories(BEFORE
        ${CMAKE_SOURCE_DIR}/mocks )

    add_library( mockqcrilNrcutils-headers INTERFACE)

target_include_directories(mockqcrilNrcutils-headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
    )

set( TARGET_MOCK_LIBCUTILS mockqcrilNrcutils )

set( MOCK_CUTILS_SOURCES
        mockproperties.c
        )

# build library
add_library ( ${TARGET_MOCK_LIBCUTILS} SHARED
        ${MOCK_CUTILS_SOURCES}
        )

target_link_libraries(${TARGET_MOCK_LIBCUTILS}
    PRIVATE ${GLIB_LIBRARIES}
    PUBLIC mockqcrilNrcutils-headers
    )

set_target_properties(${TARGET_MOCK_LIBCUTILS} PROPERTIES
        LINK_FLAGS "-Wl,-z,defs"
        VERSION 1.0.0
        SOVERSION 1)

# install to target
install ( TARGETS ${TARGET_MOCK_LIBCUTILS} mockqcrilNrcutils-headers EXPORT mockqcrilNrcutilsTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(mockqcrilNrcutils_VERSION 1.0.0)
include (CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mockqcrilNrcutils/mockqcrilNrcutilsConfigVersion.cmake"
    VERSION ${mockqcrilNrcutils_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(EXPORT mockqcrilNrcutilsTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/mockqcrilNrcutils/mockqcrilNrcutils.cmake"
    NAMESPACE QtiRil::
    )

configure_file(cmake/mockqcrilNrcutilsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/mockqcrilNrcutils/mockqcrilNrcutilsConfig.cmake"
    COPYONLY
    )

set(ConfigPackageLocation lib/cmake/mockqcrilNrcutils)
install(EXPORT mockqcrilNrcutilsTargets
    FILE mockqcrilNrcutilsTargets.cmake
    NAMESPACE QtiRil::
    DESTINATION
    ${ConfigPackageLocation}
    )

install(
    FILES
    cmake/mockqcrilNrcutilsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/mockqcrilNrcutils/mockqcrilNrcutilsConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel
    )

