include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/modules/uim/src/
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(qmi-framework)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

# Set target library name as qcrilNrQmiModule
set(TARGET_LIBQCRILNR_QMI_MODULE qcrilNrQmiModule)

# Library source files
file(GLOB libqcrilNrQmiModule_src "src/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRILNR_QMI_MODULE} SHARED
    "${libqcrilNrQmiModule_src}"
)


target_link_libraries(${TARGET_LIBQCRILNR_QMI_MODULE} qtimutex qcrilFramework qcrilNrLogger qmi
    qcrilNrUtils qmi_cci qmi_client_helper qmi_client_qmux qmiservices qmi_encdec qmiidl ${GLIB_LIBRARIES})

set_target_properties(${TARGET_LIBQCRILNR_QMI_MODULE} PROPERTIES
                        LINK_FLAGS "-Wl,-z,defs"
                        VERSION 1.0.0
                        SOVERSION 1)

install ( TARGETS ${TARGET_LIBQCRILNR_QMI_MODULE}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
