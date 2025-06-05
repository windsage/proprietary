include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(libcutils)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqtimutex
set(TARGET_LIBQTIMUTEX qtimutex)

# Library header files
file(GLOB  qtimutex_lib_headers "include/*.h")

# Library source files

file(GLOB libqtimutex_src "src/*.cpp")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "aarch64")
    else()
        set(ARCH "arm")
    endif()
endif()

if(ARCH MATCHES "aarch64")
    list(REMOVE_ITEM libqtimutex_src "${CMAKE_CURRENT_SOURCE_DIR}/src/Backtrace_others.cpp")
else()
    list(REMOVE_ITEM libqtimutex_src "${CMAKE_CURRENT_SOURCE_DIR}/src/Backtrace_arm64.cpp")
endif()

# Build libqtimutex shared library
add_library (${TARGET_LIBQTIMUTEX} SHARED
    "${libqtimutex_src}"
)

set_target_properties(${TARGET_LIBQTIMUTEX} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qtimutex_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#target_include_directories(${TARGET_LIBQTIMUTEX} PUBLIC ${qtimutex_headers})

target_link_libraries(${TARGET_LIBQTIMUTEX} pthread)

install ( TARGETS ${TARGET_LIBQTIMUTEX}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

