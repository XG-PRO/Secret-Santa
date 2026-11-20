cmake_minimum_required(VERSION 3.20)
project(SecretSanta VERSION 2.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# FLTK is fetched and linked statically, so the app is a single self-contained file.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
foreach(opt FLTK_BUILD_TEST FLTK_BUILD_EXAMPLES FLTK_BUILD_FLUID FLTK_BUILD_FLTK_OPTIONS FLTK_BUILD_GL FLTK_BUILD_FORMS
            FLTK_MSVC_RUNTIME_DLL FLTK_BACKEND_WAYLAND FLTK_USE_DBUS FLTK_USE_PANGO FLTK_OPTION_SVG FLTK_OPTION_PRINT_SUPPORT)
  set(${opt} OFF CACHE BOOL "" FORCE)
endforeach()
include(FetchContent)
FetchContent_Declare(fltk GIT_REPOSITORY https://github.com/fltk/fltk.git GIT_TAG release-1.4.5 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(fltk)

add_executable(santa WIN32 MACOSX_BUNDLE src/main.cpp)
target_link_libraries(santa PRIVATE fltk::fltk)
set_target_properties(santa PROPERTIES
  OUTPUT_NAME "Secret Santa"
  MACOSX_BUNDLE_INFO_PLIST "${fltk_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in"  # adds Retina support
  MACOSX_BUNDLE_BUNDLE_NAME "Secret Santa"
  MACOSX_BUNDLE_GUI_IDENTIFIER "com.xg-pro.secret-santa"
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
if(MINGW)
  target_link_options(santa PRIVATE -static -s)
elseif(UNIX AND NOT APPLE)
  target_link_options(santa PRIVATE -static-libstdc++ -static-libgcc -s)
endif()
