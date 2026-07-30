cmake_minimum_required(VERSION 3.20)
project(homm2 CXX)

option(HOMM2_32BIT "Build for a 32-bit target" ON)
set(HOMM2_PLATFORM "SDL3" CACHE STRING "Platform implementation")
set(HOMM2_FFMPEG_ROOT "" CACHE PATH "FFmpeg installation")

set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(HOMM2_32BIT AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_compile_options(-m32)
  add_link_options(-m32)
endif()

file(GLOB_RECURSE HOMM2_GAME_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/BASE/*.cpp
  ${CMAKE_SOURCE_DIR}/src/SOURCE/*.cpp
  ${CMAKE_SOURCE_DIR}/src/EDITOR/*.cpp)

file(GLOB HOMM2_PLATFORM_COMMON CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/PLATFORM/*.cpp)
file(GLOB HOMM2_PLATFORM_SYSTEM CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/PLATFORM/${HOMM2_PLATFORM}/*.cpp)
file(GLOB HOMM2_PLATFORM_WIN32 CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/PLATFORM/WIN32/*.cpp)

if(NOT HOMM2_PLATFORM_SYSTEM)
  message(FATAL_ERROR "Unknown HOMM2_PLATFORM: ${HOMM2_PLATFORM}")
endif()

add_library(homm2_platform STATIC
  ${HOMM2_PLATFORM_COMMON}
  ${HOMM2_PLATFORM_SYSTEM})
target_include_directories(homm2_platform PRIVATE
  ${CMAKE_SOURCE_DIR}/include)

if(HOMM2_PLATFORM STREQUAL "SDL3")
  if(EMSCRIPTEN)
    find_package(SDL3 CONFIG REQUIRED)
    set(FFMPEG_INCLUDE_DIR ${HOMM2_FFMPEG_ROOT}/include)
    foreach(library avformat avcodec swresample avutil)
      set(FFMPEG_${library}_LIBRARY
        ${HOMM2_FFMPEG_ROOT}/lib/lib${library}.a)
      list(APPEND HOMM2_FFMPEG_LIBRARIES ${FFMPEG_${library}_LIBRARY})
    endforeach()
    target_include_directories(homm2_platform PUBLIC ${FFMPEG_INCLUDE_DIR})
    target_link_libraries(homm2_platform PUBLIC SDL3::SDL3)
  else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
      libavcodec libavformat libavutil libswresample)
    set(HOMM2_FFMPEG_LIBRARIES PkgConfig::FFMPEG)
    target_link_libraries(homm2_platform PUBLIC PkgConfig::SDL3)
  endif()
endif()

add_executable(homm2
  ${HOMM2_GAME_SOURCES}
  ${HOMM2_PLATFORM_WIN32})

target_include_directories(homm2 PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/PLATFORM/WIN32)

target_compile_definitions(homm2 PRIVATE
  _X86_
  WIN32
  _WINDOWS)

if(NOT WIN32)
  target_compile_definitions(homm2 PRIVATE
    __cdecl=
    __stdcall=
    __fastcall=)
endif()

target_compile_options(homm2 PRIVATE
  -Wall
  -Wno-unused-parameter
  -Wno-sign-compare
  -Wno-unused-variable
  -Wno-unused-but-set-variable
  -Wno-parentheses
  -Wno-write-strings
  -Wno-invalid-offsetof
  -Wno-format-security
  -Wno-error=format-security
  -Wno-enum-conversion
  -Wno-narrowing
  -fpermissive)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(homm2 PRIVATE -Wno-class-memaccess)
endif()

if(EMSCRIPTEN)
  target_compile_options(homm2 PRIVATE -Wno-deprecated-literal-operator)
  set_target_properties(homm2 PROPERTIES SUFFIX ".html")
  target_link_options(homm2 PRIVATE
    "SHELL:-sASYNCIFY=1"
    "SHELL:-sALLOW_MEMORY_GROWTH=1"
    "SHELL:-sMAXIMUM_MEMORY=1073741824"
    "SHELL:-sFORCE_FILESYSTEM=1"
    "SHELL:-sEXPORTED_RUNTIME_METHODS=callMain,FS,IDBFS,addRunDependency,removeRunDependency"
    "SHELL:--shell-file=${CMAKE_SOURCE_DIR}/web/shell.html"
    "SHELL:-lidbfs.js")
elseif(UNIX)
  target_link_options(homm2 PRIVATE -rdynamic)
endif()

target_link_libraries(homm2 PRIVATE homm2_platform ${HOMM2_FFMPEG_LIBRARIES})

if(WIN32)
  target_link_libraries(homm2 PRIVATE ws2_32)
endif()

list(LENGTH HOMM2_GAME_SOURCES HOMM2_GAME_SOURCE_COUNT)
message(STATUS "homm2: ${HOMM2_GAME_SOURCE_COUNT} game units, ${HOMM2_PLATFORM}")
