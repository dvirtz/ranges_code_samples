cmake_minimum_required(VERSION 3.10)

project(
  RangeAlgorithmsTalk 
  LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
  set(ENV{CONAN_CMAKE_TOOLCHAIN_FILE} ${CMAKE_TOOLCHAIN_FILE})
  set(ENV{CONAN_CMAKE_GENERATOR} ${CMAKE_GENERATOR})
endif()

# has no effect currently
# https://github.com/conan-io/cmake-conan/issues/176
conan_add_remote(
  NAME onebaseroach 
  URL https://api.bintray.com/conan/y4n9squared/onebaseroach)

conan_add_remote(
  NAME mputsz
  URL https://api.bintray.com/conan/mpusz/conan-mpusz)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
  find_program(CONAN conan.bat)
endif()

set(DEPENDENCIES 
      range-v3/0.9.0@ericniebler/stable
      Catch2/2.9.2@catchorg/stable
      google_benchmark/1.5.0@onebaseroach/stable
      nanorange/f48db7b7@tcbrindle/testing
)

if((CMAKE_CXX_COMPILER_ID STREQUAL GCC) OR (CMAKE_CXX_COMPILER_ID STREQUAL Clang))
  list(APPEND DEPENDENCIES 
        cmcstl2/2019.08.07@mpusz/stable)
endif()

conan_cmake_run(REQUIRES ${DEPENDENCIES}
                CONAN_COMMAND ${CONAN}
                GENERATORS cmake_paths
                BUILD missing
                SETTINGS compiler.cppstd=${CMAKE_CXX_STANDARD})

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(range-v3 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(cmcstl2 CONFIG)
find_package(nanorange CONFIG REQUIRED)

option(RUN_TESTS_POSTBUILD OFF)

add_subdirectory(example)
add_subdirectory(test)
add_subdirectory(benchmarks)
