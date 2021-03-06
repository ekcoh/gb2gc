// Copyright(C) 2019 - 2020 H�kan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include "gb2gc.h"

struct option
{
    using parser = std::function<int(const gb2gc::span<const char*>& args)>;

    char        flag;        // option flag
    std::string description; // option description
    bool        required;    // is required option?
    unsigned    group;       // is option part of a group with id
    bool        need_arg;    // is this option followed by an argument
    bool        parsed;      // has option already been parsed?
    int         occurance;   // number of occurrences
    parser      parse;       // the function which parses the option
};

bool operator==(const option& o1, const option& o2)
{
   return o1.flag == o2.flag;
}

bool operator!=(const option& o1, const option& o2)
{
   return !(o1 == o2);
}

bool has_parsed_group_flag(unsigned group, const gb2gc::span<option>& options)
{
   for (auto i = 0u; i < options.count; ++i)
   {
      if (options[i].group == group && options[i].parsed)
         return true;
   }
   return false;
}

std::string get_group_flags(unsigned group, const gb2gc::span<option>& options)
{
   std::string group_flags;
   for (auto i = 0u; i < options.count; ++i)
   {
      if (options[i].group == group)
      {
         if (!group_flags.empty())
            group_flags += ", ";
         group_flags += '-';
         group_flags += options[i].flag;
      }
   }
   return group_flags;
}

bool is_option(const char* arg)
{
   return (arg[0] == '-' && arg[1] != '\0' && arg[2] == '\0');
}

const std::vector<gb2gc::selector> gb2gc::options::default_selectors(
   {  // based on default metrics from Google benchmark
      selector("name"),		// X (key)
      selector("cpu_time"),	// Y
      selector("real_time")	// Z
   });

gb2gc::options::options()
{ }

const std::string&
gb2gc::options::in_file() const
{
   return in_file_;
}

const std::string&
gb2gc::options::out_file() const
{
   return out_file_;
}

const std::string&
gb2gc::options::filter() const
{
   return filter_;
}

bool
gb2gc::options::has_filter() const
{
   return !filter_.empty();
}

const gb2gc::googlechart_dom_options&
gb2gc::options::dom_options() const
{
   return gc_dom_options_;
}

const gb2gc::googlechart_options&
gb2gc::options::chart_options() const
{
   return gc_options_;
}

const gb2gc::googlechart::visualization
gb2gc::options::chart_type() const
{
   return gc_type_;
}

const std::vector<gb2gc::selector>&
gb2gc::options::selectors() const
{
   if (selectors_.empty())
      return default_selectors;
   return selectors_;
}

int
gb2gc::options::parse_size(unsigned& dst, const char* arg)
{
   try
   {
      dst = static_cast<unsigned>(std::stol(arg));
   }
   catch (std::invalid_argument& e)
   {
      return show_error(e.what());
   }
   catch (std::out_of_range& e)
   {
      return show_error(e.what());
   }
   return 0;
}

int
gb2gc::options::parse_chart_type(const char* arg)
{
   if (strcmp(arg, "bar") == 0)
      this->gc_type_ = gb2gc::googlechart::visualization::bar;
   else if (strcmp(arg, "histogram") == 0)
      this->gc_type_ = gb2gc::googlechart::visualization::histogram;
   else if (strcmp(arg, "line") == 0)
      this->gc_type_ = gb2gc::googlechart::visualization::line;
   else if (strcmp(arg, "scatter") == 0)
      this->gc_type_ = gb2gc::googlechart::visualization::scatter;
   else
      return show_error("Invalid chart type value: '" + std::string(arg) + "'\n");
   return 0;
}

int
gb2gc::options::parse_legend(const char* arg)
{
   if (strcmp(arg, "none") == 0)
      this->gc_options_.legend = gb2gc::googlechart_options::position::none;
   else if (strcmp(arg, "left") == 0)
      this->gc_options_.legend = gb2gc::googlechart_options::position::left;
   else if (strcmp(arg, "top") == 0)
      this->gc_options_.legend = gb2gc::googlechart_options::position::top;
   else if (strcmp(arg, "right") == 0)
      this->gc_options_.legend = gb2gc::googlechart_options::position::right;
   else if (strcmp(arg, "bottom") == 0)
      this->gc_options_.legend = gb2gc::googlechart_options::position::bottom;
   else
      return show_error("Invalid chart type value: '" + std::string(arg) + "'\n");
   return 0;
}

int
gb2gc::options::parse_selector(const span<const char*>& args)
{
   for (auto& arg : args)
   {
      selectors_.emplace_back(selector(arg));
   }
   return 0;
}

int
gb2gc::options::parse_filter(const char* arg)
{
   filter_ = arg;
   return 0;
}

int gb2gc::options::parse(int argc, const char* argv[])
{
    if (argc <= 0)
        return show_error("Missing command-line arguments.");

    using type = gb2gc::googlechart::visualization;

    option opts[] =
    {
        option{ 'c', "Chart type.", 
            true, 0, true, false, 1, [&](const span<const char*>& args)
            { return this->parse_chart_type(args[0]); } },
        option{ 'f', "Filter benchmarks.",
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { return this->parse_filter(args[0]); } },
        option{ 'h', "Chart height.",
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { return this->parse_size(gc_dom_options_.height, args[0]); } },
        option{ 'i', "Input file.", 
            true, 0, true, false, 1, [&](const span<const char*>& args)
            { in_file_ = args[0]; return 0; } },
        option{ 'l', "Legend definition.", 
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { return this->parse_legend(args[0]); } },
        option{ 'o', "Output file.", 
            true, 0, true, false, 1, [&](const span<const char*>& args)
            { out_file_ = args[0]; return 0; } },
        option{ 's', "Define data selectors (default is 'name', 'real_time', 'cpu_time')",
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { return parse_selector(args); } },
        option{ 't', "Chart title.",
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { this->gc_options_.title = args[0]; return 0; } },
        option{ 'w', "Chart width.",
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { return this->parse_size(gc_dom_options_.width, args[0]); } },
        option{ 'x', "X-axis title.", 
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { this->gc_options_.horizontal_axis.title = args[0]; return 0; } },
        option{ 'y', "Y-axis title.",
            false, 0, true, false, 1, [&](const span<const char*>& args)
            { this->gc_options_.vertical_axis.title = args[0]; return 0; } }
    };

   auto options = make_span(&opts[0], sizeof(opts) / sizeof(option));
   for (auto i = 1; i < argc;)
   {
      auto arg = argv[i];
      if (!is_option(arg))
         return show_error("Expected option flag, found: " + std::string(arg));
      if (arg[2] != '\0')
         return show_error("Unrecognized option '" + std::string(arg) + "'");
      auto it = std::find(options.begin(), options.end(), option{ arg[1] });
      if (it == options.end())
         return show_error("Unrecognized option '" + std::string(arg) + "'");
      if (!it->need_arg)
      {
         it->parse(make_span<const char*>(nullptr, 0));
         it->parsed = true;
         ++i;
         continue;
      }

      auto first = i + 1;
      auto last = first;
      while (last < argc && !is_option(argv[last]))
         ++last;
      if (last == first)
      {
         std::string message("Missing argument for option -");
         message += it->flag;
         return show_error(message);
      }

      if (it->group != 0 && has_parsed_group_flag(it->group, options))
      {
         return show_error("Only one option of " + get_group_flags(it->group, options) + " may be specified.");
      }

      auto parse_error = it->parse(make_span(&argv[first], last - first));
      if (parse_error)
         return parse_error;
      it->parsed = true;

      i = last;
   }

   for (auto& opt : opts)
   {
      if (opt.required && !opt.parsed)
      {
         if (opt.group != 0)
         {
            if (has_parsed_group_flag(opt.group, options))
               continue;

            std::string message("One of the required options " + get_group_flags(opt.group, options) + " needs to be specified.");
            return show_error(message);
         }
         std::string message("Missing required option -");
         message += opt.flag;
         return show_error(message);
      }
   }

   return 0;
}

void gb2gc::options::print_usage(const char* cmd)
{
   // Arguments are IEEE 1003.1 compliant: 
   // http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html
   std::cout << "Google Benchmark to Google Chart Converter (GB2GC) v0.1.0\n";
   std::cout << "Tutorial available at: https://github.com/ekcoh/gb2gc \n\n";
   std::cout << "Usage:\n" << "  ";
   if (cmd)
      std::cout << cmd;
   std::cout << "-c type[-f filter][-l legend]|-s[-h height]-i in_file[-n name...][-o out_file][-t title][-v][-w width]\n\n"
      "Options:\n"
      "  -c               Chart type.\n"
      "  -f               Filter benchmarks.\n"
      "  -h               Chart height.\n"
      "  -i               Input file.\n"
      "  -l               Optional legend definition.\n"
      "  -n               Define benchmark parameter names.\n"
      "  -o               Optional output file.\n"
      "  -t               Optional chart title.\n"
      "  -s               Define data selectors (default is 'name', 'real_time', 'cpu_time')\n"
      "  -v               Optionally open and visualize chart directly after generation.\n"
      "  -w               Optional chart width.\n"
      "  -x               Optional x-axis title.\n"
      "  -y               Optional y-axis title.\n"
      "\n"
      "Arguments:\n"
      "  filter           Benchmark name to be matched. Wildcards ('*') can be used.\n"
      "  height           The height of the chart in pixels.\n"
      "  in_file          The input benchmark JSON file path.\n"
      "  legend           Legend position, one of 'none', 'left', 'top', 'right', 'bottom'. Defaults to 'none'"
      "  out_file         The output file path. Defaults to working directory.\n"
      "  title            The title of the chart.\n"
      "  type             The chart type. One of 'bar', 'line', 'scatter'.\n"
      "  width            The width of the chart in pixels.\n"
      "\n";
}

int gb2gc::options::show_error(const std::string& message, const char* cmd)
{
   std::cerr << "Error: " << message << "\n\n";
   print_usage(cmd);
   return 1;
}