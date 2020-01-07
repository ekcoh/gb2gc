#ifndef GBM2GC_SELECTOR_H
#define GBM2GC_SELECTOR_H

#include "gbm2gc.h"
#include "chart/variant.h"

gbm2gc::selector::selector(const std::string& name) :
	key_(name), param_index_((std::numeric_limits<unsigned>::max)())
{
	auto splits = detail::split(name, '/');
	if (splits.size() == 2)
	{
		key_ = splits[0];
		param_index_ = std::stol(splits[1]);
	}
}

html::variant
gbm2gc::selector::operator()(const nlohmann::json& benchmark) const
{
	auto node = benchmark.find(key_);
	if (node == benchmark.end())
		throw std::runtime_error("Benchmark do not have any key '" + key_ + "'");
	if (node->is_string())
	{
		// extract benchmark parameter if parameterized selector
		if (is_parameterized())
		{
			auto splits = detail::split(node->get<std::string>(), '/');
			auto param = splits[param_index_];
			auto param_value = std::stold(param);
			return html::variant(param_value);
		}

		// strip prefix
		auto s = node->get<std::string>();
		const auto prefix = std::string("BM_");
		if (s.rfind(prefix, 0) == 0)
		{
			const auto plen = prefix.length();
			return html::variant(s.substr(plen, s.length() - plen));
		}

		return html::variant(s);
	}
	return html::variant(node->get<long double>());
}

const std::string&
gbm2gc::selector::key() const
{
	return key_;
}

bool
gbm2gc::selector::is_parameterized() const
{
	return param_index_ != (std::numeric_limits<unsigned>::max)();
}

unsigned
gbm2gc::selector::param_index() const
{
	return param_index_;
}

// TODO MOVE
std::vector<std::string>
gbm2gc::detail::split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	if (!s.empty())
	{
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
			tokens.push_back(token);
	}
	return tokens;
}

#endif // GBM2GC_SELECTOR_H