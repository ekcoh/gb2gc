# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

cmake_minimum_required (VERSION 3.11)

include(FetchContent)   # Requires CMake 3.11+

###################################################################################################
# Download and unpack JSON for Modern C++ at configure time if not already available.
# If made available by parent project use that version and configuration instead.
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.7.3
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
  set(JSON_Install OFF CACHE BOOL "" FORCE)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR})
endif()

###################################################################################################
# Download and unpack variant-lite at configure time if not already available.
# If made available by parent project use that version and configuration instead.
FetchContent_Declare(
  variant_lite
  GIT_REPOSITORY https://github.com/martinmoene/variant-lite.git
  GIT_TAG        v1.2.1
)
FetchContent_GetProperties(variant_lite)
if(NOT variant_lite_POPULATED)
  FetchContent_Populate(variant_lite)
  add_subdirectory(${variant_lite_SOURCE_DIR} ${variant_lite_BINARY_DIR})
endif()

###################################################################################################
# Download and unpack optional-lite at configure time if not already available.
# If made available by parent project use that version and configuration instead.
#FetchContent_Declare(
#  optional_lite
#  GIT_REPOSITORY https://github.com/martinmoene/optional-lite.git
#  GIT_TAG        v3.2.0
#)
#FetchContent_GetProperties(optional_lite)
#if(NOT optional_lite_POPULATED)
# FetchContent_Populate(optional_lite)
#  add_subdirectory(${optional_lite_SOURCE_DIR} ${optional_lite_BINARY_DIR})
#endif()

###################################################################################################
# gb2gc project
project(gb2gc
	VERSION 0.1.0
	DESCRIPTION "Basic command-line tool to convert Google Benchmark json output to Google Charts"
	HOMEPAGE_URL "https://github.com/ekcoh/gb2gc"
	LANGUAGES CXX
)

# CMake functions to simplify usage
include(cmake/add_benchmark.cmake)

# Define a upper case variable to carry upper case project name for variable definitions
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UCASE)

# Project options
option(GB2GC_BUILD_TESTS    "Enable building tests" ON)
option(GB2GC_CODE_COVERAGE  "Enable the code coverage build option." OFF)
option(GB2GC_BUILD_ZIP      "Enable to generate ZIP distribution." OFF)
option(GB2GC_BUILD_EXAMPLES "Enable building examples" ON)

if (NOT GB2GC_BUILD_TESTS AND GB2GC_CODE_COVERAGE)
	message(FATAL_ERROR "Cannot generate code coverage without building tests. "
    "Set GB2GC_BUILD_TESTS to ON in order to build tests.")
endif()

# Use a variable for source (instead of target_sources from src) only to allow
# unit testing headers and translation units directly which is more convenient
# a CLI tool with plenty of internal complexity.
set(GB2GC_SOURCE_FILES 
	"${CMAKE_CURRENT_LIST_DIR}/src/gb2gc.h"
	"${CMAKE_CURRENT_LIST_DIR}/src/gb2gc.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/options.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/selector.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/chart.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/data_set.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/dom.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/dom.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/variant.h"   
)

###################################################################################################
# gb2gc executable target
add_executable(${PROJECT_NAME}
	"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
    ${GB2GC_SOURCE_FILES}
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

    # Make executable not depend on run-time dlls for simpler distribution.
    #target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:/MT>)
    # Use a multi-threaded statically-linked runtime library for better portability
    set_property(TARGET ${PROJECT_NAME} 
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_link_libraries(${PROJECT_NAME}
	PRIVATE nlohmann_json::nlohmann_json
	PRIVATE nonstd::variant-lite
)

if (GB2GC_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

if (GB2GC_BUILD_EXAMPLES)
	add_subdirectory(example)
endif()

# Custom target to create a ZIP package for distribution
add_custom_target(${PROJECT_NAME}_zip_package
	COMMAND ${CMAKE_COMMAND} -E echo "Creating zip distribution..."
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/README.md" ${CMAKE_CURRENT_BINARY_DIR}/.
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" ${CMAKE_CURRENT_BINARY_DIR}/.
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/THIRD-PARTY-LICENSES" ${CMAKE_CURRENT_BINARY_DIR}/.
	COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${PROJECT_NAME}_v${PROJECT_VERSION}.zip" --format=zip
		"$<TARGET_FILE_NAME:${PROJECT_NAME}>"
		"${CMAKE_CURRENT_BINARY_DIR}/LICENSE"
		"${CMAKE_CURRENT_BINARY_DIR}/THIRD-PARTY-LICENSES"
		"${CMAKE_CURRENT_BINARY_DIR}/README.md"
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${PROJECT_NAME}
	COMMENT "Creating ZIP distribution..."
	VERBATIM
)
