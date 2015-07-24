set(llceflib_host_SOURCE_FILES llceflib_host.cpp)

add_executable(llceflib_host WIN32 ${llceflib_host_SOURCE_FILES} )

target_link_libraries(llceflib_host ${CEF_LIBS})

add_dependencies(llceflib_host llceflib)