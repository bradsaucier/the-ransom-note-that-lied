cmake_minimum_required(VERSION 3.24...4.0)

project(
  RansomNoteThatLied
  VERSION 1.0.0
  DESCRIPTION "A secure, benign text provenance checker"
  LANGUAGES CXX
)

option(RNTL_BUILD_TESTS "Build the GoogleTest suite" ON)
option(RNTL_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(RNTL_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

function(rntl_harden_target target_name)
  target_compile_features(${target_name} PUBLIC cxx_std_20)

  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4 /permissive- /Zc:__cplusplus /utf-8)
    if(RNTL_WARNINGS_AS_ERRORS)
      target_compile_options(${target_name} PRIVATE /WX)
    endif()
  else()
    target_compile_options(
      ${target_name}
      PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wformat=2
    )
    if(RNTL_WARNINGS_AS_ERRORS)
      target_compile_options(${target_name} PRIVATE -Werror)
    endif()
  endif()

  if(RNTL_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
      target_compile_options(
        ${target_name}
        PRIVATE
          -fsanitize=address,undefined
          -fno-omit-frame-pointer
          -fno-sanitize-recover=all
      )
      target_link_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    else()
      message(FATAL_ERROR "RNTL_ENABLE_SANITIZERS requires GCC or Clang")
    endif()
  endif()
endfunction()

add_library(ransom_note_core src/analyzer.cpp)
target_include_directories(ransom_note_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
rntl_harden_target(ransom_note_core)

add_executable(ransom-note-checker src/main.cpp)
target_link_libraries(ransom-note-checker PRIVATE ransom_note_core)
rntl_harden_target(ransom-note-checker)

if(RNTL_BUILD_TESTS)
  include(CTest)
  include(FetchContent)

  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
    URL_HASH SHA256=63b9c77751a5b8f492486005f67533fdc58682b67476fcb91650be5958d5195a
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(googletest)

  add_executable(
    ransom_note_tests
    tests/analyzer_tests.cpp
    tests/vector_regression_tests.cpp
  )
  target_link_libraries(ransom_note_tests PRIVATE ransom_note_core GTest::gtest_main)
  target_compile_definitions(
    ransom_note_tests
    PRIVATE RNTL_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
  )
  rntl_harden_target(ransom_note_tests)

  include(GoogleTest)
  gtest_discover_tests(
    ransom_note_tests
    DISCOVERY_MODE PRE_TEST
    DISCOVERY_TIMEOUT 30
  )
endif()
