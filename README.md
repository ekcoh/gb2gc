[![Build status](https://ci.appveyor.com/api/projects/status/rw18p9p9h924o4qo?svg=true)](https://ci.appveyor.com/project/ekcoh/gb2gc)<a href="https://scan.coverity.com/projects/ekcoh-gb2gc"><img alt="Coverity Scan Build Status"  src="https://scan.coverity.com/projects/20779/badge.svg"/></a>

# gb2gc (Google Benchmark to Google Chart)

## Introduction

This repository provides utilities for converting [Google Benchmark](https://github.com/google/benchmark)
JSON output into [Google Charts](https://developers.google.com/chart) JavaScript and HTML for visualization purposes.
This makes it possible to e.g. incorporate generation of micro-benchmark charts
as part of automated build-time actions. CMake functions are also provided to simplify adding benchmarks
to your existing CMake projects in order to more easily integrate into your own continuous integration (CI) 
or documentation generation functions.

## Features
- CLI support for converting existing Google Benchmark JSON files to HTML.
- Support for adding CMake target to run benchmark via gb2gc_add_benchmark.
- Support for adding CMake target to convert benchmark JSON to HTML charts via gb2gc_add_benchmark.
- Support for incorporating benchmark related targets into build-time activities
  similar to how tests are supported in CMake via add_test.
- Chart customization support, including custom title, width, height, colors etc.
- Custom data slicing via the Selector concept (See below).

## Command-line usage

A command-line interface (CLI) tool is provided. The source code is written in
C++11 and should be portable (not tested outside MSVC & Windows). 
The CLI interface is provided via gb2gc binary target. Available options can be listed
by running the tool without giving any arguments, e.g.

```
> gb2gc
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

An example of how use the gb2gc command-line tool is given below which converts a benchmark 
to a bar-chart and provides a custom title. The -v options is provided to open the HTML page 
directly after successful conversion using the system default program:

```
> gb2gc -c bar benchmark.txt -o benchmark.html -t "My benchmark"
```

Which shows the following HTML:

![gb2gc CLI example chart output](https://user-images.githubusercontent.com/8974064/75090534-21c6f900-5564-11ea-956a-5dc788324a7f.gif)

## CMake usage

Using gb2gc from within CMake is even easier than using it from command-line. Just setup a benchmark project as usual
as described by [Google Benchmark documentation](https://github.com/google/benchmark) and use utility functions
gb2gc_add_benchmark(...) and gb2gc_add_benchmark_chart(...) to setup custom targets at configure-time.

The gb2gc_add_benchmark(...) function can be used to setup a run target for an existing
Google Benchmark executable target:

```
gb2gc_add_benchmark(TARGET my_benchmark OUT my_benchmark.json)
```

The script above will setup a custom target 'my_benchmark_run' which can be built to generate 'my_benchmark.json'.
Since we are not specifying the working directory it defaults to CMAKE_CURRENT_BINARY_DIR.

We can then use gb2gc_add_benchmark_chart(...) to generate a HTML chart representing an interpretation
of my_benchmark.json. In case we just want illustrate all benchmarks as a bart chart we can use:

```
gb2gc_add_benchmark_chart(INPUT my_benchmark.json OUTPUT my_benchmark.html)
```

This will create a custom target 'my_benchmark_chart' which can be built to generate 'my_benchmark.html'.

A simple but fully functional example is provided in /example/01_getting_started/CMakeLists.txt
which showcases how to setup run target for a simple benchmark and generate a bar chart illustrating
execution time of memcpy for different memory block sizes.

For a complete list of CMake examples, see /examples folder.

## How to build

Just add the project as a sub-project to your existing CMake project.
Dependencies are downloaded automatically by default and built as part of the project.

## License

This project is distributed under the MIT license, see: [LICENSE](LICENSE).
