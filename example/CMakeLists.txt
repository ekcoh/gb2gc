# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

###################################################################################################
# Download and unpack Google Benchmark at configure time if not already available.
# If made available by parent project use that version and configuration instead.
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.5.0
)
FetchContent_GetProperties(benchmark)
if(NOT benchmark_POPULATED)
  FetchContent_Populate(benchmark)
  if (WIN32)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  endif()
  add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
endif()

add_subdirectory(01_getting_started)
add_subdirectory(02_selector)