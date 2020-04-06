# Example 02 - Selector
#
# Showcases how to use selectors to slice data as well as some
# chart customization options

add_executable(gb2gc_example_02_selector "../01_getting_started/main.cpp")
target_link_libraries(gb2gc_example_02_selector PRIVATE benchmark)

gb2gc_add_benchmark(
    TARGET     gb2gc_example_02_selector
    OUT        benchmark.json
)

gb2gc_add_benchmark_chart(
    TARGET     gb2gc_example_02_selector_chart
    INPUT      benchmark.json
    TITLE      "Example 02 - Selector"
    TYPE       bar
    WIDTH      1100
    HEIGHT     900
    LEGEND     right
    YAXIS      "Execution time (ns)"
    SELECT     "name" "bytes_per_second"
    FILTER     "BM_memcpy/*"
)


