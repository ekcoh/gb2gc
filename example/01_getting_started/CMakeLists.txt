# Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
# This file is subject to the license terms in the LICENSE file found in the 
# root directory of this distribution.

# Add benchmark executable (as usual)
set(BENCHMARK_TARGET ${PROJECT_NAME}_example_01_getting_started)
add_executable(${BENCHMARK_TARGET} 
    "main.cpp"
)

# Link to Google Benchmark (as usual)
target_link_libraries(${BENCHMARK_TARGET}
    PRIVATE benchmark
)

# Benchmark target depends on generator being built
#add_dependencies(${BENCHMARK_TARGET} ${PROJECT_NAME})

# Setup targets for running benchmark and generating chart
gb2gc_add_benchmark(
    TARGET      ${BENCHMARK_TARGET} 
    BM_OUT      mybenchmark.json
    HTML_OUTPUT chart.html
    
    TITLE       "My Chart"
    WIDTH        1100
    HEIGHT       900
)


