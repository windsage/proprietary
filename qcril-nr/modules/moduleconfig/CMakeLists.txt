include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

SYSR_INCLUDE_DIR(qcril-nr)

# Set target library name as libqcrilNrModuleConfigModule
set(TARGET_LIBQCRIL_MODCONFIG_MODULE qcrilNrModuleConfigModule)

# Library source files
file(GLOB libqcrilNrModuleConfigModule_src "src/*.cpp")

# Build libqcrilNrModuleConfigModule static library
add_library (${TARGET_LIBQCRIL_MODCONFIG_MODULE} MODULE
    "${libqcrilNrModuleConfigModule_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_MODCONFIG_MODULE} PRIVATE
    qcrilFramework
    qcrilNr
    QtiRilLoadable
    )

install ( TARGETS qcrilNrModuleConfigModule
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
