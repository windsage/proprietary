include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/qcril-common/logger
    ${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
    ${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/utilities/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/internal-msgs/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/qtiwakelock/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/marshalling/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/marshalling/platform/linux/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/qcril_legacy/inc
    ${CMAKE_SOURCE_DIR}/mocks
)

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --unresolved-symbols=report-all")

# Set target library name as libril
set(TARGET_LIBRIL qcrilNrril)

# Library source files
file(GLOB libril_src "ril.cpp"
    "ril_event.cpp"
    "Parcel2.cpp"
    )

# Build libril shared library
add_library (${TARGET_LIBRIL} SHARED
    "${libril_src}"
)

set_target_properties(${TARGET_LIBRIL} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(ril_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/framework
)

#target_include_directories(${TARGET_LIBRIL} PUBLIC ${ril_headers})

target_link_libraries(${TARGET_LIBRIL} PRIVATE
    QtiRilLoadable
    qtimutex
    rilqmimiscservices
    pthread
    qcrilNrLogger
    qtiwakelock
    qcrildNrUtils
    qcrilMarshal
    qcrilNr
    qcrilFramework
    qcrilInterfaces
    ril-legacy
    ${GLIB_LIBRARIES})

install ( TARGETS ${TARGET_LIBRIL}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


set(TARGET_SOCKETMODULE qcrilNrSocketModule)

file (GLOB socketmodule_src
    "RilSocketIndicationModule.cpp"
    "ril_socket.cpp"
    "ril_socket_api.cpp"
    "ril_socket/*.cpp"
    "ril_socket/voice/*.cpp"
    "ril_utils.cpp"
    "RilSocketClientController.cpp"
)

add_library (${TARGET_SOCKETMODULE} SHARED
    "${socketmodule_src}"
)

target_link_libraries(${TARGET_SOCKETMODULE} PRIVATE
    qcrilFramework
    qcrilInterfaces
    qcrilMarshal
    qtiwakelock
    qcrilNr # required for constructOemHookRaw only
    ril-legacy # required for qmi_ril_get_process_instance_id only
    qcrilNrQmiModule
    ${TARGET_LIBRIL}
    ${GLIB_LIBRARIES}
)

install ( TARGETS ${TARGET_SOCKETMODULE}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
