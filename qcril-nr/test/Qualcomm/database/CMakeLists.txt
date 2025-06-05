set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC -O3")
include_directories(BEFORE
${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/nanopb
${CMAKE_SOURCE_DIR}/qcril_qmi
${CMAKE_SOURCE_DIR}/qcril_qmi
${CMAKE_SOURCE_DIR}/qcril_qmi/utilities
${CMAKE_SOURCE_DIR}/qcril_qmi/nanopb_utils
${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
${CMAKE_SOURCE_DIR}/qcril_qmi/services
${CMAKE_SOURCE_DIR}/qcrild/include
${CMAKE_SOURCE_DIR}/qcril-common/logger
${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
${CMAKE_SOURCE_DIR}/qcril-common/internal-msgs/inc
${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
${CMAKE_SOURCE_DIR}/include
)
find_package (qcrilNrLogger)

set(QtiRilDatabaseTest_VERSION 1.0.0)

add_library( QtiRilDatabaseTests SHARED)
target_link_libraries( QtiRilDatabaseTests
    PRIVATE
        qcrilNrLogger
    )

target_sources( QtiRilDatabaseTests
    PRIVATE
        "src/dummy.cpp"
    )

add_executable( QtiRilDatabaseTest)
target_sources ( QtiRilDatabaseTest
    PRIVATE
        "src/TestInit.cpp"
    )

target_link_libraries( QtiRilDatabaseTest
    PRIVATE
    ril-db
    gtest
    QtiRilDatabaseTests
    QtiRilTestMain
    qcrilFramework
    qcrilInternalMsgs
    )
install ( TARGETS QtiRilDatabaseTests QtiRilDatabaseTest EXPORT QtiRilDatabaseTestsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


include (CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilDatabaseTests/QtiRilDatabaseTestsConfigVersion.cmake"
    VERSION ${QtiRilDatabaseTest_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

set(ConfigPackageLocation lib/cmake/QtiRilDatabaseTests)
export(EXPORT QtiRilDatabaseTestsTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/QtiRilDatabaseTests/QtiRilDatabaseTestsTargets.cmake"
    NAMESPACE QtiRil::
    )

configure_file(cmake/QtiRilDatabaseTestsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilDatabaseTests/QtiRilDatabaseTestsConfig.cmake"
    COPYONLY
    )

install(
    FILES
    cmake/QtiRilDatabaseTestsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilDatabaseTests/QtiRilDatabaseTestsConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel
    )
install ( EXPORT QtiRilDatabaseTestsTargets
    FILE QtiRilDatabaseTestsTargets.cmake
    NAMESPACE QtiRil::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtiRilDatabaseTests
    )
