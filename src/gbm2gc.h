#ifndef GBM2GC_H
#define GBM2GC_H

#include "chart.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace gbm2gc
{
	static constexpr int ERROR_NO_ERROR = 0;
	static constexpr int ERROR_INVALID_ARGUMENT = 1;

	namespace detail
	{
		std::vector<std::string> split(const std::string& s, char delimiter);
	}

	/**
		* @brief Simple span type to enable template generalization.
		* @tparam T The element type.
		*/
	template<class T>
	struct span final
	{
		/** @brief The iterator type */
		using iterator_type = T * ;

		/** @brief Iterator to the first element of the span. */
		iterator_type begin() { return values; }

		/** @brief Iterator to one past the last element of the span. */
		iterator_type end() { return &values[count]; }

		/** @brief Iterator to the first element of the span. */
		iterator_type begin() const { return values; }

		/** @brief Iterator to one past the last element of the span. */
		iterator_type end() const { return &values[count]; }

		/**
			* @brief Overloaded subscript operator to direct access an element.
			* @param[in] index The index of the element to be accessed.
			* @return Reference to the element.
			*/
		inline const T& operator[](size_t index) const noexcept
		{
			assert(index < count && "span index out of range");
			return values[index];
		}

		T*     values;
		size_t count;
	};

	/**
		* @brief Creates a span from a contiguous raw array.
		* @tparam T The element type.
		* @param[in] ptr Pointer to the first element of the raw array.
		* @param[in] n   The number of contiguous elements.
		* @return span representing the raw array.
		*/
	template<class T>
	inline span<T> make_span(T* ptr, size_t n)
	{
		return span<T>{ ptr, n };
	}

	/**
		* @brief Provides the means of selecting data.
		*/
	class selector final
	{
	public:
		/**
			* @brief Constructs a selector that selects data with the given name.
			* @param[in] key The key of the data to be selected.
			* @remarks The @p key can be a regular key string or of the form
			*          'BM_Identifier/<index>' where BM_Identifier is the name of the
			*          benchmark and <index> is the index of the benchmark parameter
			*          to be selected.
			*/
		selector(const std::string& key);

		/**
			* @brief Selects data from the given benchmark.
			* @param[in] benchmark The benchmark from which to select data.
			* @return Variant data type representing the data.
			*/
        gb2gc::variant operator()(const nlohmann::json& benchmark) const;
			
		/**
			* @brief Returns the name of the associated key.
			* @return Key name.
			*/
		const std::string& key() const;

		/**
			* @brief Returns whether this selector is parameterized or not.
			* @return true if parameterized, else false.
			*/
		bool is_parameterized() const;

		/** 
			* @brief Returns the associated parameter index if this selector is
			*        a parameterized selector, i.e. is_parameterized() returns true.
			* @return Parameter index if parameterized, else the result is undefined.
			*/
		unsigned param_index() const;
		
	private:
		std::string key_;
		unsigned param_index_;
	};

	/**
		* @brief Provides the means of parsing and reading command line options.
		*/
	class options
	{
	public:
		/** @brief Constructs a default initialized options object. */
		options();

		/** 
			* @brief Parses the given command line arguments and returns
			*        a system-specific error code.
			* @param[in] argc The number of input arguments.
			* @param[in] argv The command-line arguments.
			* @return Zero if successful or a system-specific error code.
			*/
		int parse(int argc, const char* argv[]);

		const std::string& in_file() const;
		const std::string& out_file() const;
		bool has_filter() const;
		const std::string& filter() const;
		const gb2gc::googlechart_dom_options& dom_options() const;
		const gb2gc::googlechart_options& chart_options() const;
		const gb2gc::googlechart::visualization chart_type() const;
		const std::vector<selector>& selectors() const;
		
	private:
		void print_usage(const char* cmd);
		int show_error(const std::string& message, const char* cmd = nullptr);
			
		int parse_size(unsigned& dst, const char* arg);
		int parse_chart_type(const char* arg);
		int parse_legend(const char* arg);
		int parse_filter(const char* arg);
		int parse_selector(const span<const char*>& args);

		std::string in_file_;
		std::string out_file_;

		std::string filter_;

        gb2gc::googlechart_options gc_options_;
        gb2gc::googlechart_dom_options gc_dom_options_;
        gb2gc::googlechart::visualization gc_type_;
		std::vector<selector> selectors_;
		static const std::vector<selector> default_selectors;
	};

	///**
	// * @brief Provides the means of generating HTML graphs from options
	// *        and Google Benchmark JSON output.
	// */
	//class generator
	//{
	//public:
	//	int operator()(const options& options);
	//private:
	//	int operator()(const options& options, const nlohmann::json& bm_result);
	//};

	/**
		* @brief Based on the given options and JSON data parses and formats the data
		*        into a chart-compatible data-set.
		* @remarks Note that the input file options of options is ignored by this function.
		* @param[in] options The options.
		* @param[in] bm_result The JSON data.
		* @return Data set that can be converted into a chart.
		*/
    gb2gc::data_set parse_data(const options& options, const nlohmann::json& bm_result);

	void write_chart(const options& options, const gb2gc::data_set& data_set);

	/**
		* @brief Parses a Google benchmark JSON file.
		* @param[in] file Path to the JSON file to be parsed.
		* @return json object.
		*/
	nlohmann::json parse_json(const std::string& file);

	/**
		* @brief Runs the Google benchmark-to-chart converter based on command-line
		*        arguments and return a system-specific error code.
		* @param[in] argc The number of input arguments.
		* @param[in] argv The command-line arguments.
		* @return Zero if successful or a non-zero system-specific error code on failure.
		*/
	int run(int argc, const char* argv[]);
} // namespace gb2gc

#endif // GBM2GC_H