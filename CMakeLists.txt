cmake_minimum_required(VERSION 3.20)
project(homm2 CXX)

option(HOMM2_32BIT "Build for a 32-bit target" ON)
set(HOMM2_PLATFORM "SDL3" CACHE STRING "Platform implementation")

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
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
  pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec libavformat libavutil libswresample)
  target_link_libraries(homm2_platform PUBLIC PkgConfig::SDL3)
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
  _WINDOWS
  __cdecl=
  __stdcall=
  __fastcall=)

target_compile_options(homm2 PRIVATE
  -Wall
  -Wno-unused-parameter
  -Wno-sign-compare
  -Wno-unused-variable
  -Wno-unused-but-set-variable
  -Wno-parentheses
  -Wno-write-strings
  -Wno-class-memaccess
  -Wno-invalid-offsetof
  -Wno-format-security
  -Wno-error=format-security
  -Wno-enum-conversion
  -Wno-narrowing
  -fkeep-inline-functions
  -fpermissive)

target_link_options(homm2 PRIVATE -rdynamic)
target_link_libraries(homm2 PRIVATE homm2_platform PkgConfig::FFMPEG)

list(LENGTH HOMM2_GAME_SOURCES HOMM2_GAME_SOURCE_COUNT)
message(STATUS "homm2: ${HOMM2_GAME_SOURCE_COUNT} game units, ${HOMM2_PLATFORM}")
