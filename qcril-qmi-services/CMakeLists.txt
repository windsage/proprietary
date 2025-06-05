cmake_minimum_required(VERSION 2.8.9)

project (qcril-nr)

include(GNUInstallDirs)
include(FindPkgConfig)

# Supported configurations
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
  )

set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} CACHE STRING "Available build configurations." FORCE)

macro(SYSR_INCLUDE_DIR subdir)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I =/usr/include/${subdir}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I =/usr/include/${subdir}")
endmacro()

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(qmi)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC -fvisibility=default")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC -fvisibility=default")
add_compile_options("-fvisibility=default")

# Set target library name as libqcrilFramework
set(TARGET_LIBQCRIL_QMI_SERVICES qcrilQmiServices)

# Library source files
file(GLOB libqcrilqmiservices_src "*.c")

#exclude access_terminal_service_v01.c as this is built from qmi project
file (GLOB exclude_srcs "access_terminal_service_v01.c")
list(REMOVE_ITEM libqcrilqmiservices_src ${exclude_srcs})


# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_QMI_SERVICES} STATIC
    "${libqcrilqmiservices_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_QMI_SERVICES})

install ( TARGETS ${TARGET_LIBQCRIL_QMI_SERVICES}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
