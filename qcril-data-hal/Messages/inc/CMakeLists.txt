cmake_minimum_required(VERSION 2.8.9)

project (qcril-data-messages-headers)

include(GNUInstallDirs)
include(FindPkgConfig)

add_definitions(-Dstrlcpy=g_strlcpy)
add_definitions(-Dstrlcat=g_strlcat)

# Supported configurations
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
   )

set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} CACHE STRING "Available build configurations." FORCE)

add_definitions(-DRIL_FOR_MDM_LE)
add_definitions(-DRIL_FOR_LOW_RAM)

file(GLOB libqcrilMessageHeaders "Interfaces/request/*.h"
    "Interfaces/sync/*.h" "Interfaces/UnSolMessages/*.h"
    "Internal/DataInternal/sync/*.h" "Internal/DataInternal/request/*.h"
    "Internal/DataInternal/event/*.h" "Internal/DataInternal/UnSolMessages/*.h"
    "Internal/InterModule/UnSolMessages/*.h" "Internal/InterModule/sync/*.h"
    "Internal/InterModule/request/*.h"
    )
# Set target library name as libqcrilDataInterfaces
set(TARGET_LIBQCRILDATAMESSAGEHEADERS qcrilDataMessageHeaders)

add_library (${TARGET_LIBQCRILDATAMESSAGEHEADERS} INTERFACE
    "${libqcrilMessageHeaders}"
)

install ( TARGETS ${TARGET_LIBQCRILDATAMESSAGEHEADERS}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

