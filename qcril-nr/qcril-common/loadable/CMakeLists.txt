find_package (qcrilNrLogger)

set(QtiRilLoadable_VERSION 1.0.0)

add_library( QtiRilLoadable-headers INTERFACE)

target_include_directories(QtiRilLoadable-headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include/qtiril-loadable/inc>
    )

add_library( QtiRilLoadable SHARED)
target_link_libraries( QtiRilLoadable
    PRIVATE
        dl
        qcrilNrLogger
    PUBLIC
        QtiRilLoadable-headers
    )

target_sources( QtiRilLoadable
    PRIVATE
        "src/linux.cpp"
        "src/PluginManager.cpp"
    )
install ( TARGETS QtiRilLoadable-headers EXPORT QtiRilLoadableTargets
    INCLUDES DESTINATION include
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

install ( TARGETS QtiRilLoadable EXPORT QtiRilLoadableTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

include (CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilLoadable/QtiRilLoadableConfigVersion.cmake"
    VERSION ${QtiRilLoadable_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(EXPORT QtiRilLoadableTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/QtiRilLoadable/QtiRilLoadable.cmake"
    NAMESPACE QtiRil::
    )

configure_file(cmake/QtiRilLoadableConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/QtiRilLoadable/QtiRilLoadableConfig.cmake"
    COPYONLY
    )

set(ConfigPackageLocation lib/cmake/QtiRilLoadable)
install(EXPORT QtiRilLoadableTargets
    FILE QtiRilLoadableTargets.cmake
    NAMESPACE QtiRil::
    DESTINATION
    ${ConfigPackageLocation}
    )

install(
    FILES
        cmake/QtiRilLoadableConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/QtiRilLoadable/QtiRilLoadableConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
    COMPONENT
        Devel
    )
