cmake_minimum_required(VERSION 2.8.11)
project(llceflib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
set(CEF_LIBS debug ${CMAKE_CURRENT_SOURCE_DIR}/../stage/lib/debug/libcef_dll_wrapper.lib
             optimized ${CMAKE_CURRENT_SOURCE_DIR}/../stage/lib/release/libcef_dll_wrapper.lib
             debug ${CMAKE_CURRENT_SOURCE_DIR}/../stage/bin/debug/libcef.lib
             optimized ${CMAKE_CURRENT_SOURCE_DIR}/../stage/bin/release/libcef.lib)
endif(MSVC)

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
llrenderhandler.h
llschemehandler.h
)

add_library(llceflib STATIC ${llceflib_SOURCE_FILES} ${llceflib_HEADER_FILES})

target_link_libraries(llceflib ${CEF_LIBS})
					  
set_target_properties(llceflib PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

add_subdirectory(llceflib_host)
add_subdirectory(tests/win32gl)