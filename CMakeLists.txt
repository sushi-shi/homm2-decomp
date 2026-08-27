cmake_minimum_required(VERSION 3.20)
project(homm2 C CXX)

include(CTest)

option(HOMM2_32BIT "Build for a 32-bit target" ON)
option(HOMM2_PLATFORM_WARNINGS_AS_ERRORS "Treat portable platform warnings as errors" OFF)
set(HOMM2_PLATFORM "SDL3" CACHE STRING "Platform implementation")
set(HOMM2_FFMPEG_ROOT "" CACHE PATH "FFmpeg installation")
set(HOMM2_BZIP2_ROOT "" CACHE PATH "libbz2 installation")

set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Retail pointer width and packed layouts are load-bearing, so the game is
# built 32-bit. On a 64-bit host that means -m32, and a toolchain that cannot
# do it should say so here rather than on the first source file.
if(HOMM2_32BIT AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_compile_options(-m32)
  add_link_options(-m32)

  include(CMakePushCheckState)
  include(CheckCXXSourceCompiles)

  cmake_push_check_state(RESET)
  set(CMAKE_REQUIRED_FLAGS -m32)
  check_cxx_source_compiles("int main() { return 0; }" HOMM2_HAVE_32BIT_TOOLCHAIN)
  cmake_pop_check_state()

  if(NOT HOMM2_HAVE_32BIT_TOOLCHAIN)
    message(FATAL_ERROR
      "This is a 32-bit build and ${CMAKE_CXX_COMPILER_ID} cannot produce a "
      "32-bit binary here.\n"
      "Install the multilib toolchain: gcc-multilib and g++-multilib on "
      "Debian/Ubuntu, glibc-devel.i686 and libstdc++-devel.i686 on Fedora.\n"
      "Nix users get all of it from `nix develop`.")
  endif()
endif()

file(GLOB_RECURSE HOMM2_GAME_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/BASE/*.cpp
  ${CMAKE_SOURCE_DIR}/src/SOURCE/*.cpp
  ${CMAKE_SOURCE_DIR}/src/EDITOR/*.cpp
  ${CMAKE_SOURCE_DIR}/src/IRONFIST/*.cpp)

if(EMSCRIPTEN)
  set(HOMM2_EXECUTABLE WASM)
elseif(WIN32)
  set(HOMM2_EXECUTABLE WINDOWS)
else()
  set(HOMM2_EXECUTABLE POSIX)
endif()

file(GLOB HOMM2_EXECUTABLE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/EXECUTABLE/*.cpp
  ${CMAKE_SOURCE_DIR}/src/EXECUTABLE/${HOMM2_EXECUTABLE}/*.cpp)

file(GLOB HOMM2_PLATFORM_COMMON CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/PLATFORM/*.cpp)
file(GLOB HOMM2_PLATFORM_SYSTEM CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/PLATFORM/${HOMM2_PLATFORM}/*.cpp)
if(NOT HOMM2_PLATFORM_SYSTEM)
  message(FATAL_ERROR "Unknown HOMM2_PLATFORM: ${HOMM2_PLATFORM}")
endif()

add_library(homm2_platform STATIC
  ${HOMM2_PLATFORM_COMMON}
  ${HOMM2_PLATFORM_SYSTEM})
target_include_directories(homm2_platform PRIVATE
  ${CMAKE_SOURCE_DIR}/include)

file(GLOB HOMM2_LUA_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/vendor/lua-5.3.3/*.c)
add_library(homm2_lua STATIC ${HOMM2_LUA_SOURCES})

add_library(homm2_tinyxml2 STATIC
  ${CMAKE_SOURCE_DIR}/vendor/tinyxml2-7.1.0/tinyxml2.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(homm2_platform PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow)
  if(HOMM2_PLATFORM_WARNINGS_AS_ERRORS)
    target_compile_options(homm2_platform PRIVATE -Werror)
  endif()
endif()

if(HOMM2_PLATFORM STREQUAL "SDL3")
  # A statically linked SDL3 drags in system libraries of its own. That list
  # lives in its package config, which pkg-config does not carry.
  if(EMSCRIPTEN OR MINGW)
    find_package(SDL3 CONFIG REQUIRED)
    set(HOMM2_SDL3_LIBRARY SDL3::SDL3)
  else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    set(HOMM2_SDL3_LIBRARY PkgConfig::SDL3)
  endif()

  if(HOMM2_FFMPEG_ROOT)
    target_include_directories(homm2_platform SYSTEM PUBLIC ${HOMM2_FFMPEG_ROOT}/include)
    foreach(library avformat avcodec swresample avutil)
      list(APPEND HOMM2_FFMPEG_LIBRARIES
        ${HOMM2_FFMPEG_ROOT}/lib/lib${library}.a)
    endforeach()
  else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
      libavcodec libavformat libavutil libswresample)
    set(HOMM2_FFMPEG_LIBRARIES PkgConfig::FFMPEG)
  endif()

  target_link_libraries(homm2_platform PUBLIC
    ${HOMM2_SDL3_LIBRARY}
    ${HOMM2_FFMPEG_LIBRARIES})

  # pkg-config answers about the libraries it can see, which on a 64-bit
  # host are the 64-bit ones. Link something before believing it.
  if(HOMM2_32BIT AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_FLAGS -m32)
    set(CMAKE_REQUIRED_LIBRARIES ${HOMM2_SDL3_LIBRARY} ${HOMM2_FFMPEG_LIBRARIES})
    check_cxx_source_compiles([[
      #include <SDL3/SDL.h>
      extern "C" {
      #include <libavcodec/avcodec.h>
      }
      int main() { return SDL_WasInit(0) + avcodec_version(); }
    ]] HOMM2_HAVE_32BIT_LIBRARIES)
    cmake_pop_check_state()

    if(NOT HOMM2_HAVE_32BIT_LIBRARIES)
      message(FATAL_ERROR
        "SDL3 and FFmpeg were found, but not as 32-bit libraries.\n"
        "Install the 32-bit development packages: libsdl3-dev:i386 and the "
        "libav*-dev:i386 set on Debian/Ubuntu, SDL3-devel.i686 and "
        "ffmpeg-devel.i686 on Fedora.\n"
        "Nix users get all of it from `nix develop`.")
    endif()
  endif()
endif()

if(HOMM2_BZIP2_ROOT)
  add_library(Homm2BZip2::BZip2 STATIC IMPORTED)
  set_target_properties(Homm2BZip2::BZip2 PROPERTIES
    IMPORTED_LOCATION ${HOMM2_BZIP2_ROOT}/lib/libbz2.a
    INTERFACE_INCLUDE_DIRECTORIES ${HOMM2_BZIP2_ROOT}/include)
  set(HOMM2_BZIP2_LIBRARY Homm2BZip2::BZip2)
else()
  find_package(BZip2 REQUIRED)
  set(HOMM2_BZIP2_LIBRARY BZip2::BZip2)
endif()

list(REMOVE_ITEM HOMM2_GAME_SOURCES ${CMAKE_SOURCE_DIR}/src/BASE/Bzip2.cpp)
add_library(homm2_compression STATIC src/BASE/Bzip2.cpp)
target_include_directories(homm2_compression PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(homm2_compression PUBLIC ${HOMM2_BZIP2_LIBRARY})
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(homm2_compression PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Werror)
endif()

add_executable(homm2
  ${HOMM2_GAME_SOURCES}
  ${HOMM2_EXECUTABLE_SOURCES})

# PO files and runtime catalogs stay UTF-8. Original byte-encoded resources
# are transcoded only when their text fields enter the engine.
find_program(HOMM2_MSGFMT msgfmt)
find_program(HOMM2_MSGCAT msgcat)
find_program(HOMM2_XGETTEXT xgettext)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
file(GLOB HOMM2_PO_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/locales/*.po)
set(HOMM2_COMPILED_CATALOGS)
if(HOMM2_PO_FILES)
  if(NOT HOMM2_MSGFMT OR NOT HOMM2_MSGCAT OR NOT HOMM2_XGETTEXT)
    message(FATAL_ERROR
      "gettext tools msgfmt, msgcat and xgettext are required to build translations")
  endif()
  set(messageRegistry ${CMAKE_SOURCE_DIR}/locales/messages.def)
  set(registryTemplate ${CMAKE_CURRENT_BINARY_DIR}/lang/registry.pot)
  set(tableTemplate ${CMAKE_CURRENT_BINARY_DIR}/lang/tables.pot)
  set(messageTemplate ${CMAKE_CURRENT_BINARY_DIR}/lang/messages.pot)
  add_custom_command(
    OUTPUT ${registryTemplate}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/lang
    COMMAND ${HOMM2_XGETTEXT} --language=C++ --from-code=UTF-8
      --keyword=HOMM2_MESSAGE:1c,2 --keyword=HOMM2_PLURAL:1c,2,3
      --output=${registryTemplate} ${messageRegistry}
    DEPENDS ${messageRegistry}
    VERBATIM)
  add_custom_command(
    OUTPUT ${tableTemplate}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/localization_tables.py
      template ${CMAKE_SOURCE_DIR} ${tableTemplate}
    DEPENDS
      ${CMAKE_SOURCE_DIR}/tools/localization_tables.py
      ${CMAKE_SOURCE_DIR}/locales/legacy_tables.def
      ${CMAKE_SOURCE_DIR}/src/SOURCE/KB.cpp
      ${CMAKE_SOURCE_DIR}/src/SOURCE/X_GLOBAL.cpp
    VERBATIM)
  add_custom_command(
    OUTPUT ${messageTemplate}
    COMMAND ${HOMM2_MSGCAT} --use-first --output-file=${messageTemplate}
      ${registryTemplate} ${tableTemplate}
    DEPENDS ${registryTemplate} ${tableTemplate}
    VERBATIM)
  foreach(po ${HOMM2_PO_FILES})
    get_filename_component(language ${po} NAME_WE)
    set(catalog ${CMAKE_CURRENT_BINARY_DIR}/lang/${language}.mo)
    add_custom_command(
      OUTPUT ${catalog}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/lang
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/validate_catalog.py
        ${po} ${messageTemplate}
      COMMAND ${HOMM2_MSGFMT} --check --check-format --no-convert
        --output-file=${catalog} ${po}
      DEPENDS ${po} ${messageTemplate} ${CMAKE_SOURCE_DIR}/tools/validate_catalog.py
      VERBATIM)
    list(APPEND HOMM2_COMPILED_CATALOGS ${catalog})
  endforeach()
  add_custom_target(homm2_locales ALL DEPENDS ${HOMM2_COMPILED_CATALOGS})
  add_dependencies(homm2 homm2_locales)
endif()

target_include_directories(homm2 PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/vendor/lua-5.3.3
  ${CMAKE_SOURCE_DIR}/vendor/tinyxml2-7.1.0)

if(NOT WIN32)
  target_compile_definitions(homm2 PRIVATE
    __cdecl=
    __stdcall=
    __fastcall=)
endif()

target_compile_options(homm2 PRIVATE
  -Wall
  -Werror=cast-align
  -Werror=implicit-fallthrough
  -Werror=non-virtual-dtor
  -Werror=null-dereference
  -Werror=old-style-cast
  -Werror=overloaded-virtual
  -Werror=shadow
  -Werror=type-limits
  -Werror=unused-label
  -Werror=unused-parameter
  -Werror=sign-compare
  -Werror=unused-variable
  -Werror=unused-but-set-variable
  -Werror=parentheses
  -Werror=write-strings
  -Werror=char-subscripts
  -Werror=delete-non-virtual-dtor
  -Werror=format
  -Wno-invalid-offsetof
  -Werror=format-security
  -Werror=uninitialized
  -Werror=enum-conversion
  -Werror=narrowing)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(homm2 PRIVATE
    -Wno-class-memaccess
    -Werror=array-bounds
    -Werror=duplicated-cond
    -Werror=format-overflow
    -Werror=format-truncation
    -Werror=logical-op
    -Werror=maybe-uninitialized
    -Werror=stringop-truncation)
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

target_link_libraries(homm2 PRIVATE
  homm2_platform
  homm2_lua
  homm2_tinyxml2
  homm2_compression
  ${HOMM2_FFMPEG_LIBRARIES})

if(BUILD_TESTING AND NOT WIN32 AND NOT EMSCRIPTEN)
  add_executable(homm2_bzip2_test
    tools/bzip2_test.cpp)
  target_include_directories(homm2_bzip2_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  target_link_libraries(homm2_bzip2_test PRIVATE homm2_compression)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(homm2_bzip2_test PRIVATE
      -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Werror)
  endif()
  add_test(NAME bzip2 COMMAND homm2_bzip2_test)

  add_executable(homm2_localization_encoding_test
    tools/localization_encoding_test.cpp
    src/BASE/Utf8.cpp
    src/SOURCE/LegacyText.cpp
    src/SOURCE/PluralRules.cpp)
  target_include_directories(homm2_localization_encoding_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR})
  target_compile_options(homm2_localization_encoding_test PRIVATE
    -ffunction-sections -fdata-sections)
  target_link_options(homm2_localization_encoding_test PRIVATE
    -Wl,--gc-sections)
  add_test(NAME localization_encoding COMMAND homm2_localization_encoding_test)
  add_executable(homm2_resource_profile_test
    tools/resource_profile_test.cpp
    src/SOURCE/ResourceProfile.cpp)
  target_include_directories(homm2_resource_profile_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  add_test(NAME resource_profile COMMAND homm2_resource_profile_test)
  add_executable(homm2_tick_deadline_test tools/tick_deadline_test.cpp)
  target_include_directories(homm2_tick_deadline_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  add_test(NAME tick_deadline COMMAND homm2_tick_deadline_test)
  add_executable(homm2_legacy_presentation_test tools/legacy_presentation_test.cpp)
  target_include_directories(homm2_legacy_presentation_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  add_test(NAME legacy_presentation COMMAND homm2_legacy_presentation_test)
  add_executable(homm2_filesystem_test tools/filesystem_test.cpp)
  target_include_directories(homm2_filesystem_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  target_link_libraries(homm2_filesystem_test PRIVATE homm2_platform)
  add_test(NAME filesystem COMMAND homm2_filesystem_test)
  add_executable(homm2_platform_primitives_test tools/platform_primitives_test.cpp)
  target_include_directories(homm2_platform_primitives_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR})
  target_link_libraries(homm2_platform_primitives_test PRIVATE homm2_platform)
  add_test(NAME platform_primitives COMMAND homm2_platform_primitives_test)
  add_executable(homm2_platform_backend_test tools/platform_backend_test.cpp)
  target_include_directories(homm2_platform_backend_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  target_link_libraries(homm2_platform_backend_test PRIVATE homm2_platform)
  add_test(NAME platform_backend COMMAND homm2_platform_backend_test)
  add_executable(homm2_platform_atexit_test tools/platform_atexit_test.cpp)
  target_include_directories(homm2_platform_atexit_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  target_link_libraries(homm2_platform_atexit_test PRIVATE homm2_platform)
  add_test(NAME platform_atexit COMMAND homm2_platform_atexit_test)
  add_executable(homm2_palette_bank_test tools/palette_bank_test.cpp)
  target_include_directories(homm2_palette_bank_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  add_test(NAME palette_bank COMMAND homm2_palette_bank_test)
  add_executable(homm2_mapcell_layout_test tools/mapcell_layout_test.cpp)
  target_include_directories(homm2_mapcell_layout_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include)
  add_test(NAME mapcell_layout COMMAND homm2_mapcell_layout_test)
  add_test(
    NAME agg_manifest
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/test_agg_manifest.py)
  add_test(
    NAME catalog_validation
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/test_validate_catalog.py)
  add_test(
    NAME ironfist_interface_audit
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/test_ironfist_interface_audit.py)
  add_test(
    NAME ironfist_hook_contract
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/test_ironfist_hook_contract.py)
endif()

if(WIN32)
  target_link_libraries(homm2 PRIVATE ws2_32)
  # A game, not a console program: retail came up without one behind it.
  set_target_properties(homm2 PROPERTIES WIN32_EXECUTABLE ON)
endif()

install(TARGETS homm2 RUNTIME DESTINATION bin)
if(HOMM2_COMPILED_CATALOGS)
  install(FILES ${HOMM2_COMPILED_CATALOGS} DESTINATION share/homm2/lang)
endif()

list(LENGTH HOMM2_GAME_SOURCES HOMM2_GAME_SOURCE_COUNT)
message(STATUS "homm2: ${HOMM2_GAME_SOURCE_COUNT} game units, ${HOMM2_PLATFORM}, ${HOMM2_EXECUTABLE}")
