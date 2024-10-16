cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mmdsrv LANGUAGES CXX)

include(cmake/BuildConfig.cmake)
include(cmake/CPM.cmake)

# Section: Dependencies
# FMT for formatting stuff
CPMAddPackage("gh:fmtlib/fmt#11.0.2")
# lockfree queue (and other stuff, but we user the queue)
CPMAddPackage("gh:DNedic/lockfree#2.0.9")
# CXXOpts for reading cmdline options
CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.2.0
  OPTIONS
    "CXXOPTS_BUILD_EXAMPLES Off"
    "CXXOPTS_BUILD_TESTS Off"
)
# Boost ASIO without the rest of Boost
find_package(Threads REQUIRED)
CPMAddPackage("gh:chriskohlhoff/asio#asio-1-31-0@1.31.0")
if(asio_ADDED)
  add_library(asio INTERFACE)
  target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)
  target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
  target_link_libraries(asio INTERFACE Threads::Threads)
  if(WIN32)
    # macro see @ https://stackoverflow.com/a/40217291/1746503
    macro(get_win32_winnt version)
      if(CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
        string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
        # Check for Windows 10, b/c we'll need to convert to hex 'A'.
        if("${verMajor}" MATCHES "10")
          set(verMajor "A")
          string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
        endif("${verMajor}" MATCHES "10")
        # Remove all remaining '.' characters.
        string(REPLACE "." "" ver ${ver})
        # Prepend each digit with a zero.
        string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
      endif()
    endmacro()

    if(NOT DEFINED _WIN32_WINNT)
      get_win32_winnt(ver)
      set(_WIN32_WINNT ${ver})
    endif()

    message(STATUS "Set _WIN32_WINNET=${_WIN32_WINNT}")

    target_compile_definitions(asio INTERFACE _WIN32_WINNT=${_WIN32_WINNT} WIN32_LEAN_AND_MEAN)
  endif()
endif()

# Project
add_executable(mmdsrv "src/main.cpp")

file(GLOB_RECURSE SOURCES "src/*")
file(GLOB_RECURSE HEADERS "include/*")

target_sources(mmdsrv PRIVATE ${SOURCES} PUBLIC ${HEADERS})

target_include_directories(mmdsrv PUBLIC "include")
target_link_libraries(mmdsrv fmt::fmt lockfree asio cxxopts)

CPMAddPackage("gh:catchorg/Catch2@3.7.0")
file(GLOB_RECURSE TESTS "test/*")
add_executable(test_mmdsrv ${TESTS})
target_sources(test_mmdsrv PUBLIC ${HEADERS})
target_include_directories(test_mmdsrv PUBLIC "include")
target_link_libraries(test_mmdsrv Catch2::Catch2WithMain fmt::fmt lockfree asio cxxopts)
enable_testing()
add_test(test_mmdsrv test_mmdsrv)
