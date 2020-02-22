# Example 01 - Getting started
#
# Showcases creating a Google Benchmark binary program and automatically
# creating run target and chart conversion target.

set(example_01_getting_started)
add_executable(example_01_getting_started "main.cpp")
target_link_libraries(example_01_getting_started PRIVATE benchmark)

gb2gc_add_benchmark(
    TARGET     example_01_getting_started
    OUT        benchmark.json
)

gb2gc_add_benchmark_chart(
    INPUT      benchmark.json
    TITLE      "Example 01 - Getting Started"
    WIDTH      1100
    HEIGHT     900
)


