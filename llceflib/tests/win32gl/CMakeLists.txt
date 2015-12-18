set(test_win32gl_HEADER_FILES resource.h)

set(test_win32gl_SOURCE_FILES win32gl.cpp win32gl.rc)

add_executable(test_win32gl WIN32 ${test_win32gl_SOURCE_FILES} ${test_win32gl_HEADER_FILES})

target_link_libraries(test_win32gl llceflib opengl32 ${CEF_LIBS})

add_dependencies(test_win32gl llceflib llceflib_host)