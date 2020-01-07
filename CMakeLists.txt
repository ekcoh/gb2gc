cmake_minimum_required (VERSION 3.8)

###################################################################################################
# Download and unpack googletest at configure time

configure_file(external/gtest/CMakeLists.txt.in external/gtest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/gtest-download )
if(result)
  message(FATAL_ERROR "CMake step for Google Test (gtest) failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/gtest-download )
if(result)
  message(FATAL_ERROR "Build step for Google Test (gtest) failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add gtest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/external/gtest-src
                 ${CMAKE_BINARY_DIR}/external/gtest-build
                 EXCLUDE_FROM_ALL)

#set(GTEST_ROOT "${CMAKE_BINARY_DIR}/external/googletest-src")
#set(GTEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/external/googletest-src/googletest/include")
#set(GTEST_LIBRARY "${CMAKE_BINARY_DIR}/external/googletest-build/googlemock/gtest/gtest")
#set(GTEST_MAIN_LIBRARY "${CMAKE_BINARY_DIR}/external/googletest-build/googlemock/gtest/gtest_main")

###################################################################################################
# Download and unpack googlebenchmark at configure time
configure_file(external/benchmark/CMakeLists.txt.in external/benchmark-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/benchmark-download )
if(result)
  message(FATAL_ERROR "CMake step for Google Benchmark (benchmark) failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/benchmark-download )
if(result)
  message(FATAL_ERROR "Build step for Google Benchmark (benchmark) failed: ${result}")
endif()

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
#set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

# TODO How the heck to we build benchmark with CMAKE_BUILD_TYPE=Release

#set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/external/benchmark-src
                 ${CMAKE_BINARY_DIR}/external/benchmark-build
                 EXCLUDE_FROM_ALL)

###################################################################################################
# Download and unpack json at configure time
configure_file(external/json/CMakeLists.txt.in external/json-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/json-download )
if(result)
  message(FATAL_ERROR "CMake step for json failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/json-download )
if(result)
  message(FATAL_ERROR "Build step for json failed: ${result}")
endif()

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
#set(JSON_BuildTests OFF CACHE INTERNAL "")
#set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(${CMAKE_BINARY_DIR}/external/json-src
                 ${CMAKE_BINARY_DIR}/external/json-build
                 EXCLUDE_FROM_ALL)
#add_subdirectory(${CMAKE_BINARY_DIR}/external/json-src)

###################################################################################################
# Download and unpack variant-lite at configure time
configure_file(external/variant_lite/CMakeLists.txt.in external/variant_lite-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/variant_lite-download )
if(result)
  message(FATAL_ERROR "CMake step for variant_lite failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/variant_lite-download )
if(result)
  message(FATAL_ERROR "Build step for variant_lite failed: ${result}")
endif()

add_subdirectory(${CMAKE_BINARY_DIR}/external/variant_lite-src
                 ${CMAKE_BINARY_DIR}/external/variant_lite-build
                 EXCLUDE_FROM_ALL)

# enable_testing()

#set (GTEST_ROOT "${CMAKE_BINARY_DIR}/external/googlebenchmark-src")

project(gbm2gc
	VERSION 0.0.0
	DESCRIPTION "Basic tool written in C++11 to convert Google Benchmark json output to Google Charts"
	LANGUAGES CXX
)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UCASE)

add_executable(${PROJECT_NAME}
	"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/gbm2gc.h"
	"${CMAKE_CURRENT_LIST_DIR}/src/gbm2gc.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/options.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/selector.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/chart.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/data_set.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/document.cpp"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
if(MSVC)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in 
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_link_libraries(${PROJECT_NAME}
	PRIVATE nlohmann_json::nlohmann_json
	PRIVATE variant-lite
)

enable_testing()
add_subdirectory(test)

