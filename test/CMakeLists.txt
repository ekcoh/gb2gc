# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

add_executable(gb2gc_unit_tests 
    ${GB2GC_SOURCE_FILES}
    "chart_test.cpp"
    "data_set_test.cpp"
    "dom_test.cpp" 
    "gb2gc_test.cpp"
	"main.cpp"
    "options_test.cpp"
	"selector_test.cpp"
	"variant_test.cpp"
)

target_compile_features(gb2gc_unit_tests PRIVATE cxx_std_11)
if(MSVC)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in 
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	target_compile_options(gb2gc_unit_tests  PRIVATE /W4 /WX)
else(MSVC)
	target_compile_options(gb2gc_unit_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(gb2gc_unit_tests 
	PRIVATE "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(gb2gc_unit_tests 
	PRIVATE nlohmann_json::nlohmann_json
	PRIVATE gtest
	PRIVATE variant-lite
)

# Simple function just to cut down on boilerplate in test CMakeLists.txt
function(gb2gc_add_binary_dir_copy TARGET FILENAME)
    add_custom_command(TARGET gb2gc_copy_test_data
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "${CMAKE_CURRENT_SOURCE_DIR}/data/${FILENAME}"
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/${FILENAME}"
        VERBATIM
    )
endfunction()

add_custom_target(gb2gc_copy_test_data)
gb2gc_add_binary_dir_copy(gb2gc_unit_tests benchmark1.json)
gb2gc_add_binary_dir_copy(gb2gc_unit_tests benchmark2.json)
gb2gc_add_binary_dir_copy(gb2gc_unit_tests line.html)
gb2gc_add_binary_dir_copy(gb2gc_unit_tests bar.html)
gb2gc_add_binary_dir_copy(gb2gc_unit_tests histogram.html)
add_dependencies(gb2gc_unit_tests gb2gc_copy_test_data)

add_test(NAME gb2gc_unit_tests COMMAND gb2gc_unit_tests )
