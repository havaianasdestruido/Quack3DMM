cmake_minimum_required(VERSION 3.20)
project(Quack3DMM
    VERSION 1.0.0
    LANGUAGES C CXX
    DESCRIPTION "Microsoft 3D Movie Maker - Open Source Release"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC runtime library")
    add_compile_options(
        /W3
        /wd4996
        /wd4244
        /wd4267
        /wd4311
        /wd4312
        /wd4838
        /wd4245
        /wd4305
        /wd4018
        /wd4365
        /wd4100
        /wd4189
        /wd4706
        /wd4702
        /wd4127
        /wd4514
        /wd4710
        /wd4711
        /wd4505
        /wd4820
        /wd5045
    )
    add_compile_options(/EHsc /GR)
endif()

if(WIN32)
    add_compile_definitions(WIN32 WIN _WINDOWS _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS STRICT)
endif()

add_compile_definitions(UNICODE _UNICODE)

include(CTest)

option(BUILD_LEGACY_ENGINE "Build the legacy 3D Movie Maker engine libraries" OFF)

# Lua (always needed by script/sandbox/q3d_ui)
add_subdirectory(third_party/luasrc)

# Modern modding layer (vfs / q3m / mod_manager) - always built
add_subdirectory(SRC/ENGINE)

# Legacy SOC engine (pre-C++98 code, still being modernized). Off by default
if(BUILD_LEGACY_ENGINE)
    include_directories(
        ${CMAKE_SOURCE_DIR}/INC
        ${CMAKE_SOURCE_DIR}/BREN/INC
        ${CMAKE_SOURCE_DIR}/KAUAI/SRC
    )
    add_subdirectory(BREN)
    add_subdirectory(KAUAI)
    add_subdirectory(SRC/STUDIO)
endif()

# --- Modding subsystem tests (standalone, no legacy engine deps) ---
add_executable(test_vfs TESTS/test_vfs.cpp)
target_link_libraries(test_vfs PRIVATE vfs)
add_test(NAME test_vfs COMMAND test_vfs)
set_tests_properties(test_vfs PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(test_mods TESTS/test_manifest.cpp)
target_link_libraries(test_mods PRIVATE mod_manager mod_core vfs)
add_test(NAME test_mods COMMAND test_mods)

add_executable(test_event TESTS/test_event_bus.cpp)
target_link_libraries(test_event PRIVATE event)
add_test(NAME test_event COMMAND test_event)

add_executable(test_hotreload TESTS/test_hotreload.cpp)
target_link_libraries(test_hotreload PRIVATE hotreload)
add_test(NAME test_hotreload COMMAND test_hotreload)

add_executable(test_render TESTS/test_shader.cpp)
target_link_libraries(test_render PRIVATE render)
add_test(NAME test_render COMMAND test_render)

add_executable(test_audio TESTS/test_audio.cpp)
target_link_libraries(test_audio PRIVATE audio)
add_test(NAME test_audio COMMAND test_audio)

add_executable(test_import TESTS/test_image.cpp)
target_link_libraries(test_import PRIVATE import)
add_test(NAME test_import COMMAND test_import)

add_executable(test_texture_baker TESTS/test_texture_baker.cpp)
target_link_libraries(test_texture_baker PRIVATE import)
add_test(NAME test_texture_baker COMMAND test_texture_baker)

add_executable(test_actor TESTS/test_actor_def.cpp)
target_link_libraries(test_actor PRIVATE actor)
add_test(NAME test_actor COMMAND test_actor)
set_tests_properties(test_actor PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(test_script TESTS/test_lua.cpp)
target_link_libraries(test_script PRIVATE script lua mod_core)
add_test(NAME test_script COMMAND test_script)

add_executable(test_sandbox TESTS/test_sandbox.cpp)
target_link_libraries(test_sandbox PRIVATE sandbox lua)
add_test(NAME test_sandbox COMMAND test_sandbox)

add_executable(test_ui TESTS/test_ui.cpp)
target_link_libraries(test_ui PRIVATE q3d_ui)
add_test(NAME test_ui COMMAND test_ui)