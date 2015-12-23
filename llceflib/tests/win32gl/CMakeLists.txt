set(test_win32gl_HEADER_FILES resource.h)

set(test_win32gl_SOURCE_FILES win32gl.cpp win32gl.rc)

add_executable(test_win32gl WIN32 ${test_win32gl_SOURCE_FILES} ${test_win32gl_HEADER_FILES})

target_link_libraries(test_win32gl llceflib opengl32 ${CEF_LIBS})

if (MSVC)
  set_target_properties(test_win32gl PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif (MSVC)

add_dependencies(test_win32gl llceflib llceflib_host)

if (MSVC)
  EMBED_MANIFEST(test_win32gl 1)
endif (MSVC)
