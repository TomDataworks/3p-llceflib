set(llceflib_host_SOURCE_FILES llceflib_host.cpp)
add_executable(llceflib_host WIN32 MACOSX_BUNDLE ${llceflib_host_SOURCE_FILES} )
add_dependencies(llceflib_host llceflib)

if (MSVC)
  target_link_libraries(llceflib_host ${CEF_LIBS})
  set_target_properties(llceflib PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  EMBED_MANIFEST(llceflib_host 1)
elseif (UNIX)
  target_link_libraries(llceflib_host ${CEF_LIBS})
elseif (APPLE)
  include(BundleUtilities)

  set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../stage/lib/release)
  find_library(CEF_FRAMEWORK "Chromium Embedded Framework"
      PATHS ${LIB_PATH} REQUIRED)
  find_library(LIBCEF_LIBRARY cef_dll_wrapper
      PATHS ${LIB_PATH} REQUIRED)
  mark_as_advanced(CEF_FRAMEWORK LIBCEF_LIBRARY)

  target_link_libraries(llceflib_host ${CEF_LIBS} ${CEF_FRAMEWORK} ${LIBCEF_LIBRARY})
  set_target_properties(llceflib_host PROPERTIES 
      OUTPUT_NAME "LLCefLib Helper"
      MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in")

  # This command kinda sucks, but well... I'm entirely sick of this.
  add_custom_command(TARGET llceflib_host
    POST_BUILD
    COMMAND install_name_tool -change "@executable_path/Chromium Embedded Framework"
            "@executable_path/../../../../Frameworks/Chromium Embedded Framework.framework/Chromium Embedded Framework"
            "${CMAKE_CURRENT_SOURCE_DIR}/../bin/Release/LLCefLib Helper.app/Contents/MacOS/LLCefLib Helper"
    VERBATIM
    )
endif ()

