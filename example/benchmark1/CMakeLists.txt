# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

# Benchmark executable
add_executable(${PROJECT_NAME}_benchmark1
	"main.cpp"
)

target_compile_features(${PROJECT_NAME}_benchmark1 PRIVATE cxx_std_11)
if(MSVC)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in 
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	target_compile_options(${PROJECT_NAME}_benchmark1 PRIVATE /W4 /WX)

	# Generate assembly for reviewing if needed
	# target_compile_options(hitlib PRIVATE /FAscu /Fa )
else(MSVC)
	target_compile_options(${PROJECT_NAME}_benchmark1 PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

# Setup project dependencies
target_link_libraries(${PROJECT_NAME}_benchmark1
	PRIVATE benchmark
)

add_dependencies(${PROJECT_NAME}_benchmark1 ${PROJECT_NAME})

# Tell CMake how to generate benchmark JSON result and setup dependencies
add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/benchmark1.json
	COMMAND $<TARGET_FILE:${PROJECT_NAME}_benchmark1> 
	        --benchmark_counters_tabular=true 
			--benchmark_format=console 
			--benchmark_out=benchmark1.json
			--benchmark_out_format=json
	DEPENDS ${PROJECT_NAME}_benchmark1
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	VERBATIM
)

# Tell CMake how to generate HTML chart from JSON result and setup dependencies
add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/benchmark1.html
	COMMAND $<TARGET_FILE:${PROJECT_NAME}> 
		-c bar
		-i "${CMAKE_BINARY_DIR}/benchmark1.json"
		-o "${CMAKE_BINARY_DIR}/benchmark1.html"
		-t "Benchmark 1 Example - memcpy vs. memmove for different data sizes"
		-w 1100
		-h 900
		-y "Time (nanoseconds)"
		-l right
	DEPENDS 
		${PROJECT_NAME} 
		${CMAKE_BINARY_DIR}/benchmark1.json
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	VERBATIM
)

# Add target to run benchmark
add_custom_target(run_benchmark1
	COMMAND ${CMAKE_COMMAND} -E echo "Running benchmark 1..."
	DEPENDS ${CMAKE_BINARY_DIR}/benchmark1.json
	VERBATIM
)

# Add target to generate chart
add_custom_target(generate_chart_benchmark1
	COMMAND ${CMAKE_COMMAND} -E echo "Generating chart for benchmark 1..."
	DEPENDS ${CMAKE_BINARY_DIR}/benchmark1.html 
	VERBATIM
)

message("BINARY: ${GB2GC}")
