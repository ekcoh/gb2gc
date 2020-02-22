# gb2gc (Google Benchmark to Google Chart)

## Introduction

This repository provides utilities for converting Google Benchmark JSON output
into Google Charts for visualization purposes in HTML documentation.
This makes it possible to e.g. incorporate generation of API cost benchmarks
as part of build-time generation. CMake functions to simplify adding benchmarks
to existing CMake projects are provided.

## Features

- CLI support for converting existing Google Benchmark JSON files to HTML.
- Support for adding CMake target to run benchmark via add_benchmark.
- Support for adding CMake target to convert benchmark JSON to HTML charts via add_benchmark.
- Support for incorporating benchmark related targets into build-time activities
  similar to how tests are supported in CMake via add_test.
- Chart customization support:
    - Custom chart title via TITLE argument.
    - Custom chart width via WIDTH argument.
    - Custom chart height via HEIGHT argument.
    - Custom data slicing via data Selector concept (See below).
    -

## Command-line usage

A command-line interface (CLI) tool is provided. The source code is written in
C++11 and should be portable since only dependency is STL functions. The CLI
interface is provided via gb2gc binary target. Available options can be listed
by running the tool without given any arguments:

```
> gb2gc

Error: Missing required option -c

Google Benchmark to Google Chart Converter (GB2GC) v0.1.0
Tutorial available at: https://github.com/ekcoh/gb2gc

Usage:
  -c type[-f filter]-l|-s[-h height]-i in_file[-n name...][-o out_file][-t title][-v][-w width]

Options:
  -c               Chart type.
  -f               Filter benchmarks.
  -h               Chart height.
  -i               Input file.
  -n               Define benchmark parameter names.
  -o               Optional output file.
  -t               Optional chart title.
  -s               Define data selectors (default is 'name', 'real_time', 'cpu_time')
  -v               Optionally open and visualize chart directly after generation.
  -w               Optional chart width.
  -x               Optional x-axis title.
  -y               Optional y-axis title.

Arguments:
  filter           Benchmark name to be matched. Wildcards ('*') can be used.
  height           The height of the chart in pixels.
  in_file          The input benchmark JSON file path.
  out_file         The output file path. Defaults to working directory.
  title            The title of the chart.
  type             The chart type. One of 'bar', 'line', 'scatter'.
  width            The width of the chart in pixels.
```

An example of how use the gb2gc command-line tool is given below which converts a benchmark to a bar-chart and provides a custom title. The -v options is provided to open the HTML page directly after successful conversion using the system default program:

```
> gb2gc -c bar benchmark.txt -o benchmark.html -t "My benchmark"
```

## CMake usage

TBD

https://stackoverflow.com/questions/12698003/save-a-google-chart-as-svg
https://stackoverflow.com/questions/12698003/save-a-google-chart-as-svg