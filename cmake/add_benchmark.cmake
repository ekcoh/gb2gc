# Copyright(C) 2019 - 2020 H�kan Sidenvall <ekcoh.git@gmail.com>.
#
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.
#
# See https://github.com/ekcoh/gb2gc for updates, documentation, and revision 
# history.

# gb2gc_add_benchmark
#
# Adds benchmark run target to an existing executable CMake target built on
# Google Benchmark:
#
# gb2gc_add_benchmark(
#   TARGET      target
#   OUT         benchmark_out  
#   [OUT_FORMAT benchmark_out_format]
#   [FORMAT     benchmark_format]
#   [COLOR      benchmark_color]
#   [COUNTERS_TABULAR]
#   [FILTER     regex]
#   [REPETITIONS repetitions]
#   [REPORT_AGGREGATES_ONLY]
#   [WORKING_DIRECTORY dir]
# )
#
# The given 'target' specifies a valid executable CMake target built on 
# Google Benchmark and this function setup a custom target for running that
# benchmark with the given options. The name of the generated custom target
# will be <target>_run.
#
# The options are (Also see: https://github.com/google/benchmark#command-line):
#
# COLOR
#
# COUNTERS_TABULAR
#   Print each counter as a table column. 
#   Forwards '--benchmark_counters_tabular=true' to Google Benchmark binary.
#   
# FILTER
#   Use this to only run the benchmarks that match the specified <regex>.
#   Forwards '--benchmark_filter=<regex>' to Google Benchmark binary.
#
# FORMAT
#   Forwards '--benchmark_format=<benchmark_format>' to Google Benchmark binary.
#   Set to any of <console|json|csv>. Note that json is the only option
#   currently supported by gb2gc.
#
# OUT_FORMAT
#   Forwards --benchmark_out_format=<benchmark_out_format> to Google Benchmark.
#
# OUT
#   Specifies the JSON output file to be generated by the benchmark executable.
#   If the output path is a relative path it will be interpreted as relative
#   to the working directory which defaults to the build tree current binary 
#   directory. 
#   Forwards '--benchmark_out=<benchmark_out>' to Google Benchmark.
#   
# REPETITIONS
#   By default Google Benchmark dynamically determines number fo iterations.
#   Use this option to run several times and aggregate statistical results 
#   across these repetitions.
#   Forwards '--benchmark_repetitions=<repetitions>' to Google Benchmark binary.
#
# TARGET
#   Specifies an existing CMake target representing a Google Benchmark 
#   executable.
#
# WORKING_DIRECTORY
#   Specifies another working directory than the current binary directory. 
#   All relative paths specified as arguments to this function will be 
#   interpreted as relative to this directory if specified.
#
#
function(gb2gc_add_benchmark)
   cmake_parse_arguments(
        GB2GC
        "COUNTERS_TABULAR;REPORT_AGGREGATES_ONLY"
        "TARGET;COLOR;FORMAT;OUT;OUT_FORMAT;WORKING_DIRECTORY"
        "ARGS"
        ${ARGN}
    )

    ###########################################################################
    # Assert & build benchmark arguments

    if (NOT GB2GC_TARGET)
        message(FATAL_ERROR "ERROR: Missing required option 'TARGET'")
    endif()
    if (NOT GB2GC_WORKING_DIRECTORY)
      set(GB2GC_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()

    if (NOT GB2GC_OUT) # Required
        message(FATAL_ERROR "ERROR: Missing required option 'OUT'")
    endif()
    list(APPEND GB2GC_ARGS "--benchmark_out=${GB2GC_OUT}")

    if (NOT GB2GC_OUT_FORMAT)
        list(APPEND GB2GC_ARGS "--benchmark_out_format=json")
    endif()

    if (GB2GC_FORMAT)
        list(APPEND GB2GC_ARGS "--benchmark_format=${GB2GC_FORMAT}")
    endif()

    if (GB2GC_COUNTERS_TABULAR)
        list(APPEND GB2GC_ARGS "--benchmark_counters_tabular=true")
    endif()

    if (GB2GC_COLOR)
        list(APPEND GB2GC_ARGS "--benchmark_color=${GB2GC_COLOR}")
    endif()

    if (GB2GC_REPORT_AGGREGATES_ONLY)
        list(APPEND GB2GC_ARGS "--benchmark_report_aggregates_only=true")
    endif()

    message(status ": GB2GC_ARGS=${GB2GC_ARGS}") # Verbose

    ###########################################################################
    # Custom command: generate benchmark output file by running target binary

    add_custom_command(
        OUTPUT ${GB2GC_OUT}
        COMMAND $<TARGET_FILE:${GB2GC_TARGET}> ${GB2GC_ARGS}
        DEPENDS ${GB2GC_TARGET}
        WORKING_DIRECTORY ${GB2GC_WORKING_DIRECTORY}
        VERBATIM
    )

    ###########################################################################
    # Custom target: run target binary

    add_custom_target(${GB2GC_TARGET}_run
        COMMENT "Running benchmark ${BENCHMARK_TARGET}..."
	    COMMAND ${CMAKE_COMMAND} -E echo "Running benchmark ${BENCHMARK_TARGET}..."
	    DEPENDS ${GB2GC_OUT}
	    VERBATIM
    )

endfunction()

# gb2gc_add_benchmark_chart
#
# Adds a run target that generates an HTML chart representing 
# the benchmark:
#
# gb2gc_add_benchmark_chart(
#   TARGET target
#   BM_OUT output  
#   [HTML_OUTPUT html_output]
#   [OPTIONS option1 [options2] ...]
#   [GB2GC_OPTIONS option1 [option2] ...]
#   [WORKING_DIRECTORY dir]
# )
#
# The given 'target' specifies a valid executable CMake target built on 
# Google Benchmark and this function setup a custom target for running that
# benchmark with the given options.
#
# The options are:
#
# BM_OUT
# BM_OUT_FORMAT
# BM_FORMAT
# BM_COUNTERS_TABULAR
# BM_COLOR
# BM_REPORT_AGGREGATES_ONLY
#
# HTML_OUTPUT 
#   Specifies the HTML output file to be generated with a chart representation
#   of the benchmark result. If the output path is a relative path it will be 
#   interpreted as relative to the working directory which defaults to the 
#   build tree current binary directory.  
#
# OPTIONS
#   Allows to override 
#   See: https://github.com/google/benchmark#command-line
#
# OUTPUT
#   Specifies the JSON output file to be generated by the benchmark executable.
#   If the output path is a relative path it will be interpreted as relative
#   to the working directory which defaults to the build tree current binary 
#   directory. 
#
# TARGET
#   Specifies an existing CMake target representing a Google Benchmark 
#   executable.
#
# WORKING_DIRECTORY
#   Specifies another working directory than the current binary directory. 
#   All relative paths specified as arguments to this function will be 
#   interpreted as relative to this directory if specified.
#
#
function(gb2gc_add_benchmark_chart)
   cmake_parse_arguments(
        GB2GC
        ""
        "TARGET;INPUT;OUTPUT;WORKING_DIRECTORY;TITLE;WIDTH;HEIGHT;LEGEND;TYPE;XAXIS;YAXIS;FILTER"
        "SELECT"
        ${ARGN}
    )

    ###########################################################################
    # Assert general options
    if (NOT GB2GC_WORKING_DIRECTORY)
      set(GB2GC_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if (NOT GB2GC_TARGET)
        message(FATAL_ERROR "ERROR: Missing required option 'TARGET'")
    endif()

    ###########################################################################
    # Assert HTML/chart (CHART prefixed) options

    if (NOT GB2GC_OUTPUT)
        # Default to same filename as BM_OUT but .html extension
        get_filename_component(GB2GC_OUTPUT_NAME_WE 
             ${GB2GC_INPUT} NAME_WE)
        set(GB2GC_OUTPUT "${GB2GC_OUTPUT_NAME_WE}.html")
    endif()

    #if (NOT GB2GC_ARGS)
    #  list(APPEND GB2GC_ARGS "-c" "bar")
    #  list(APPEND GB2GC_ARGS "-i" "${GB2GC_INPUT}")
    #  list(APPEND GB2GC_ARGS "-o" "${GB2GC_OUTPUT}")
    #endif()

    list(APPEND GB2GC_ARGS "-o" "${GB2GC_OUTPUT}")

    if (NOT GB2GC_INPUT)
        message(FATAL_ERROR "ERROR: Missing required option 'INPUT'")
    else()
        list(APPEND GB2GC_ARGS "-i" "${GB2GC_INPUT}")
    endif()

    if (GB2GC_TITLE)
        list(APPEND GB2GC_ARGS "-t" "${GB2GC_TITLE}")
    endif()

    if (GB2GC_WIDTH)
        list(APPEND GB2GC_ARGS "-w" "${GB2GC_WIDTH}")
    endif()
    if (GB2GC_HEIGHT)
        list(APPEND GB2GC_ARGS "-h" "${GB2GC_HEIGHT}")
    endif()
    if (GB2GC_LEGEND)
        list(APPEND GB2GC_ARGS "-l" "${GB2GC_LEGEND}")
    endif()
    if (GB2GC_TYPE)
        list(APPEND GB2GC_ARGS "-c" "${GB2GC_TYPE}")
    else()
        list(APPEND GB2GC_ARGS "-c" "bar") # Consider defaulting from CLI tool?
    endif()
    if (GB2GC_XAXIS)
        list(APPEND GB2GC_ARGS "-x" "${GB2GC_XAXIS}")
    endif()
    if (GB2GC_YAXIS)
        list(APPEND GB2GC_ARGS "-y" "${GB2GC_YAXIS}")
    endif()
    if (GB2GC_FILTER)
        list(APPEND GB2GC_ARGS "-f" "${GB2GC_FILTER}")
    endif()
    if (GB2GC_SELECT)
        list(APPEND GB2GC_ARGS "-s" "${GB2GC_SELECT}")
    endif()

    ###########################################################################
    # Custom commands

    # Add a custom command to generate HTML chart from benchmark output file
    add_custom_command(
	    OUTPUT ${GB2GC_OUTPUT}
	    COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${GB2GC_ARGS}
	    DEPENDS 
		    gb2gc 
		    ${GB2GC_INPUT}
	    WORKING_DIRECTORY ${GB2GC_WORKING_DIRECTORY}
	    VERBATIM
    )

    ###########################################################################
    # Custom targets

    # Add target to generate chart
    add_custom_target(${GB2GC_TARGET}
        COMMENT "Generating chart ${GB2GC_OUTPUT}..."
	    COMMAND ${CMAKE_COMMAND} -E echo "Generating chart..."
	    DEPENDS ${GB2GC_OUTPUT} 
	    VERBATIM
    )

endfunction()