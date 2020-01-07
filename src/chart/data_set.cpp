#include "data_set.h"

std::ostream& html::operator<<(
	std::ostream& os, const html::data_set::const_row_iterator& rit)
{
	auto it = rit.begin();
	const auto end = rit.end();
	os << '[';
	if (it != end)
	{
		os << *it;
		++it;
		for (; it != end; ++it)
			os << ',' << *it;
	}
	os << ']';
	return os;
}

std::ostream& html::operator<<(
	std::ostream& os, const html::data_set& ds)
{
	const auto cols = ds.cols();
	const auto rows = ds.rows();
	os << '[';
	for (auto col = 0u; col < cols; ++col)
	{
		if (col != 0) os << ", ";
		os << ds.get_col(col).name();
	}
	os << "]\n";
	for (auto row = 0u; row < rows; ++row)
	{
		os << '[';
		for (auto col = 0u; col < cols; ++col)
		{
			if (col != 0) os << ", ";
			os << ds.get_col(col)[row];
		}
		os << "]\n";
	}
	return os;
}