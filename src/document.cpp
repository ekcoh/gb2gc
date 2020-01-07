#include "document.h"

// Indentation formatting

std::ostream& gb2gc::operator<<(std::ostream& os, const indent& ind)
{
	const auto cnt = ind.level * ind.fmt.indentation;
	for (auto i = 0u; i < cnt; ++i)
		os << ' ';
	return os;
}

// Element

gb2gc::element::element(const std::string& name)
	: name_(name), value_(), convertible_value_(nullptr)
{ }

const std::string& 
gb2gc::element::name() const { return this->name_; }

const gb2gc::element::attribute_container&
gb2gc::element::attributes() const { return attributes_; }

const gb2gc::element::element_container&
gb2gc::element::children() const { return children_; }

gb2gc::element&
gb2gc::element::set_comment(const std::string& comment)
{
	comment_ = comment;
	return *this;
}

gb2gc::element&
gb2gc::element::set_comment(std::string&& comment)
{
	comment_ = std::move(comment);
	return *this;
}

gb2gc::element&
gb2gc::element::add_attribute(const attribute& attrib)
{
	attributes_.push_back(attrib);
	return *this;
}

gb2gc::element&
gb2gc::element::add_attribute(const std::string& key, const std::string& value)
{
	attributes_.emplace_back(key, value);
	return *this;
}

gb2gc::element&
gb2gc::element::add_attribute(attribute&& attrib)
{
	attributes_.emplace_back(std::move(attrib));
	return *this;
}

void gb2gc::element::add_value(const std::string& value)
{
	value_ = value;
}

bool gb2gc::element::is_leaf() const
{ 
	return children_.empty(); 
}

bool gb2gc::element::has_comment() const
{ 
	return !comment_.empty(); 
}

bool gb2gc::element::has_content() const
{ 
	return !value_.empty(); 
}

bool gb2gc::element::has_convertible_content() const
{ 
	return convertible_value_ != nullptr; 
}

const std::string& gb2gc::element::comment() const
{ 
	return comment_; 
}

const std::string& gb2gc::element::content() const
{ 
	return value_; 
}

void gb2gc::element::format(std::ostream& os,
	const gb2gc::format& fmt, size_t level) const
{
	convertible_value_(os, fmt, level);
}

gb2gc::element&
gb2gc::element::set_content(std::string&& value)
{
	value_ = std::move(value);
	return *this;
}

gb2gc::element&
gb2gc::element::set_content(convertible convertible)
{
	convertible_value_ = std::move(convertible);
	return *this;
}

// Detail

std::ostream& gb2gc::detail::format_attribute(
	std::ostream& os, const format&, const element::attribute& attr)
{
	os << ' ' << attr.first << "=\"" << attr.second << '"';
	return os;
}

void gb2gc::detail::write_content(
	std::ostream& os, const format& fmt, size_t level, const std::string& in)
{
	std::istringstream ss(in);
	std::string line;
	while (getline(ss, line))
		os << indent{ fmt, level } << line << '\n';
}

std::ostream& gb2gc::detail::close_element(
	std::ostream& os, const format& fmt, size_t level, const element& e)
{
	os << '\n' << fmt.indent(level) << "</" << e.name() << '>' << '\n';
	return os;
}

std::ostream& gb2gc::detail::write(
	std::ostream& os, const format& fmt, const element& e, size_t level)
{
	const auto ind = indent{ fmt, level };
	if (e.has_comment())
		os << ind << "<!-- " << e.comment() << "-->\n";

	os << ind << '<' << e.name();
	for (const auto& attrib : e.attributes())
		format_attribute(os, fmt, attrib);
	os << '>';

	if (e.has_convertible_content())
	{
		os << '\n';
		e.format(os, fmt, level + 1);
		os << '\n';
		os << ind << "</" << e.name() << '>' << '\n';
	}
	else if (e.has_content())
	{
		os << '\n';
		write_content(os, fmt, level + 1, e.content());
		os << ind << "</" << e.name() << '>' << '\n';
	}
	else if (e.children().empty())
	{
		os << "</" << e.name() << '>' << '\n';
	}
	else
	{
		os << "\n";
		for (const auto& c : e.children())
			write(os, fmt, c, level + 1);
		os << ind << "</" << e.name() << '>' << '\n';
	}
	return os;
}
