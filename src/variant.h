#ifndef GB2GC_CHART_VARIANT_H
#define GB2GC_CHART_VARIANT_H

#include <nonstd/variant.hpp>

#include <iostream>
#include <string>

namespace gb2gc
{
	/**
	 * @brief Represents a variant type for JS trivial types compatible with HTML charts.
	 */
	using variant = nonstd::variant<
		nonstd::monostate,
		short,
		unsigned short,
		int,
		unsigned int,
		long,
		unsigned long,
		long long,
		unsigned long long,
		float,
		double,
		long double,
		std::string>;

	/**
	 * @brief Convenience method for creating a null-value for variant type.
	 * @return Variant value corresponding to null representation.
	 */
	inline constexpr nonstd::monostate null_value() { return {}; }

	/**
	 * @brief Overloaded output operator writing the given variant to the
	 *        output stream.
	 * @remarks Specialized for conversion to JavaScript syntax.
	 * @param[in] os The destination output stream.
	 * @param[in] v  The variant.
	 * @return The output-stream @p os passed by reference.
	 */
	inline std::ostream& operator<<(std::ostream & os, gb2gc::variant const & v)
	{
		switch (v.index())
		{
		case 0:  os << "'null'"; break;
		case 1:  os << std::to_string(nonstd::get<1>(v)); break;
		case 2:  os << std::to_string(nonstd::get<2>(v)); break;
		case 3:  os << std::to_string(nonstd::get<3>(v)); break;
		case 4:  os << std::to_string(nonstd::get<4>(v)); break;
		case 5:  os << std::to_string(nonstd::get<5>(v)); break;
		case 6:  os << std::to_string(nonstd::get<6>(v)); break;
		case 7:  os << std::to_string(nonstd::get<7>(v)); break;
		case 8:  os << std::to_string(nonstd::get<8>(v)); break;
		case 9:  os << std::to_string(nonstd::get<9>(v)); break;
		case 10: os << std::to_string(nonstd::get<10>(v)); break;
		case 11: os << std::to_string(nonstd::get<11>(v)); break;
		case 12: os << '\'' << nonstd::get<12>(v) << '\''; break;
		default: os << "unexpected index";
		}
		return os;
	}
} // namespace gb2gc

#endif // GB2GC_CHART_VARIANT_H