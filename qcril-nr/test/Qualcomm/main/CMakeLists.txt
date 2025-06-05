find_package (qcrilNrLogger)
find_package (gtest)

set(QtiRilTestMain_VERSION 1.0.0)

add_library( QtiRilTestMain STATIC )
target_link_libraries( QtiRilTestMain
    PRIVATE
        gtest
    )

target_sources( QtiRilTestMain
    PRIVATE
        "src/main.cpp"
    )

install ( TARGETS QtiRilTestMain EXPORT QtiRilTestMainTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

include (CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilTestMain/QtiRilTestMainConfigVersion.cmake"
    VERSION ${QtiRilTestMain_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(EXPORT QtiRilTestMainTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/QtiRilTestMain/QtiRilTestMain.cmake"
    NAMESPACE QtiRil::
    )

configure_file(cmake/QtiRilTestMainConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilTestMain/QtiRilTestMainConfig.cmake"
    COPYONLY
    )

set(ConfigPackageLocation lib/cmake/QtiRilTestMain)
install(EXPORT QtiRilTestMainTargets
    FILE QtiRilTestMainTargets.cmake
    NAMESPACE QtiRil::
    DESTINATION
    ${ConfigPackageLocation}
    )

install(
    FILES
    cmake/QtiRilTestMainConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/QtiRilTestMain/QtiRilTestMainConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel
    )
