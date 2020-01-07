add_executable(${PROJECT_NAME}_unit_tests 
	"${PROJECT_SOURCE_DIR}/src/gbm2gc.h"
	"${PROJECT_SOURCE_DIR}/src/gbm2gc.cpp"
	"${PROJECT_SOURCE_DIR}/src/selector.cpp"
	"${PROJECT_SOURCE_DIR}/src/options.cpp"

	"${PROJECT_SOURCE_DIR}/src/chart/chart.cpp"
	"${PROJECT_SOURCE_DIR}/src/chart/data_set.cpp"
	"${PROJECT_SOURCE_DIR}/src/chart/document.cpp"
	
	"main.cpp" 
	"gbm2gc_test.cpp"
	"options_test.cpp"
	"selector_test.cpp"

	"chart/chart_test.cpp"
	"chart/data_set_test.cpp"
)

target_compile_features(${PROJECT_NAME}_unit_tests PRIVATE cxx_std_11)
if(MSVC)
	# CMake appends /W3 by default, and having /W3 followed by /W4 will result in 
	# cl : Command line warning D9025 : overriding '/W3' with '/W4'.  Since this is
	# a command line warning and not a compiler warning, it cannot be suppressed except
	# by fixing the command line.
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	target_compile_options(${PROJECT_NAME}_unit_tests  PRIVATE /W4 /WX)

	# Generate assembly for reviewing if needed
	# target_compile_options(hitlib PRIVATE /FAscu /Fa )
else(MSVC)
	target_compile_options(${PROJECT_NAME}_unit_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(${PROJECT_NAME}_unit_tests 
	PRIVATE "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME}_unit_tests 
	PRIVATE nlohmann_json::nlohmann_json
	PRIVATE gtest
	PRIVATE variant-lite
)

# Copy json files for test to test directory
add_custom_command(TARGET ${PROJECT_NAME}_unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/simple_bm.json"
        $<TARGET_FILE_DIR:${PROJECT_NAME}_unit_tests>)

add_custom_command(TARGET ${PROJECT_NAME}_unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/window_functions.txt"
        $<TARGET_FILE_DIR:${PROJECT_NAME}_unit_tests>)

add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${PROJECT_NAME}_unit_tests )

#enable_testing()

# Add test binary as code coverage target
#target_code_coverage(${PROJECT_NAME}_unit_tests 
#	TARGET_SOURCES ${PROJECT_NAME}
#	SOURCE_BASE_DIR ${PROJECT_SOURCE_DIR}
#)
#get_target_property(GBM2GC_SOURCES ${PROJECT_NAME} SOURCES)
#list
#message("GBM2GC_SOURCES=${GBM2GC_SOURCES}")
#add_code_coverage(
#	TARGET ${PROJECT_NAME}_unit_tests
#	SOURCES ${GBM2GC_SOURCES}
#)
# Add test binary as code coverage target
#add_code_coverage(
#	TARGET ${PROJECT_NAME}_unit_tests
#	SOURCES $<TARGET_PROPERTY:${PROJECT_NAME},SOURCES>
#		"${PROJECT_SOURCE_DIR}/src/*"
#)