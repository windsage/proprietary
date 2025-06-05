# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)
SYSR_INCLUDE_DIR(event)
SYSR_INCLUDE_DIR(sync)
SYSR_INCLUDE_DIR(qdpr)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(qmi-client-helper)
SYSR_INCLUDE_DIR(dsutils)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(data)
SYSR_INCLUDE_DIR(diag)
SYSR_INCLUDE_DIR(libxml2)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

# Set target library name as qcrilDataModule
set(TARGET_LIBQCRILDATA qcrilDataModule)

# Library source files
file(GLOB libqcrilData_src
    "../../util/src/*.cpp"
    "../../endpoints/src/*.cpp"
    "../../authmanager/src/*.cpp"
    "../../profilehandler/src/*.cpp"
    "../../callmanager/src/*.cpp"
    "../../tmdmanager/src/*.cpp"
    "../../KeepAliveManager/src/*.cpp"
    "../../networkservicehandler/src/*.cpp"
    "../../networkavailabilityhandler/src/*.cpp"
    "src/*.cpp"
    "../../lcehandler/src/*.cpp"
    )

file (GLOB exclude_srcs "../../util/src/DsiWrapperLA.cpp"
    "../../endpoints/src/VoiceCallModemEndPoint.cpp"
    "../../endpoints/src/VoiceCallModemEndPointModule.cpp"
    )

list(REMOVE_ITEM libqcrilData_src ${exclude_srcs})

# Build libqcrilDataModule shared library
add_library (${TARGET_LIBQCRILDATA} SHARED
    "${libqcrilData_src}"
)

target_link_libraries(${TARGET_LIBQCRILDATA}
    qdpr
    dsi_netctrl
    log
    libqcrilDataInterfaces
    libqcrilDataInternal
    qcrilFramework
    -Wl,--whole-archive
    qcrilNrQmiModule
    qcrilNrUtils
    ril-db
    -Wl,--no-whole-archive
    sqlite3
    qmi
    qmi_cci
    qmi_client_qmux
    qmiservices
    qmi_encdec
    qcrilInternalMsgs
    qtimutex
    dsutils
    diag
    qcrilNrLogger
    qtibus
    qmi_client_helper
    rt
    ${GLIB_LIBRARIES})

set_target_properties(${TARGET_LIBQCRILDATA} PROPERTIES
                      LINK_FLAGS "-Wl,-z,defs"
                      VERSION 1.0.0
                      SOVERSION 1)


install ( TARGETS ${TARGET_LIBQCRILDATA}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

