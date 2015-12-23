cmake_minimum_required(VERSION 3.1.0)
project(llceflib)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include("macros")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
set(CEF_LIBS debug ${CMAKE_CURRENT_SOURCE_DIR}/../cef/build/libcef_dll/Debug/libcef_dll_wrapper.lib
             optimized ${CMAKE_CURRENT_SOURCE_DIR}/../cef/build/libcef_dll/Release/libcef_dll_wrapper.lib
             debug ${CMAKE_CURRENT_SOURCE_DIR}/../cef/Debug/libcef.lib
             optimized ${CMAKE_CURRENT_SOURCE_DIR}/../cef/Release/libcef.lib)
endif(MSVC)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-x objective-c++")
endif(APPLE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cef)

set(llceflib_SOURCE_FILES
llbrowserclient.cpp
llceflib.cpp
llceflibimpl.cpp
llrenderhandler.cpp
llschemehandler.cpp
)

if(WIN32)
  list(APPEND llceflib_SOURCE_FILES llceflibimpl_win.cpp)
elseif(APPLE)
  list(APPEND llceflib_SOURCE_FILES llceflibimpl_mac.mm)
endif(WIN32)

set(llceflib_HEADER_FILES
llbrowserclient.h
llceflib.h
llceflibimpl.h
llceflibplatform.h
llrenderhandler.h
llschemehandler.h
)

add_library(llceflib STATIC ${llceflib_SOURCE_FILES} ${llceflib_HEADER_FILES})

target_link_libraries(llceflib ${CEF_LIBS})

if (MSVC)
  set_target_properties(llceflib PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif (MSVC)

add_subdirectory(llceflib_host)
add_subdirectory(tests/win32gl)
