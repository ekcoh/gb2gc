#ifndef GB2GC_HTML_DOCUMENT_H
#define GB2GC_HTML_DOCUMENT_H

#include <string>
#include <functional>
#include <ostream>
#include <sstream>
#include <vector>

namespace html
{
	// Forward declarations
	struct format;

	/**
		* @brief Represents a basic DOM element
		*/
	class element
	{
	public:
		using element_container = std::vector<element>;
		using element_iterator = typename element_container::iterator;

		using attribute = std::pair<std::string, std::string>;
		using attribute_container = std::vector<attribute>;
		using attribute_iterator = typename attribute_container::iterator;

		using convertible = std::function<void(std::ostream&, const format&, size_t level)>;

		/** @brief Represent DOM element content type */
		enum class content_type
		{
			node,
			string,
			convertible
		};

		/**
		 * @brief Constructs an element with the given name.
		 * @param[in] name The element name.
		 */
		explicit element(const std::string& name);
		~element() = default;
		element(const element& other) = delete;
		element& operator=(const element& other) = delete;
		element(element&& other) = default;
		element& operator=(element&& other) = default;

		element& set_comment(const std::string& comment);
		element& set_comment(std::string&& comment);
		element& set_content(std::string&& value);
		element& set_content(convertible convertible);

		template<class... Args>
		element& add_element(Args&&... args)
		{
			children_.emplace_back(std::forward<Args>(args)...);
			return children_[children_.size() - 1];
		}

		void add_value(const std::string& value);
		element& add_attribute(const attribute& attrib);
		element& add_attribute(const std::string& key, const std::string& value);
		element& add_attribute(attribute&& attrib);
			
		const std::string& name() const;
		const attribute_container& attributes() const;
		const element_container& children() const;
		bool is_leaf() const;
		bool has_comment() const;
		bool has_content() const;
		bool has_convertible_content() const;
		const std::string& comment() const;
		const std::string& content() const;
		void format(std::ostream& os, const format& fmt, size_t level) const;

	private:
		std::string name_;
		std::string value_;
		std::string comment_;
		convertible convertible_value_;
		std::vector<attribute> attributes_;
		std::vector<element> children_;
	};

	inline element make_element(std::string name)
	{
		return element{ name };
	}

	struct indent
	{
		const format& fmt;
		size_t level;
	};

	struct format
	{
		bool strip_comments = false;
		unsigned indentation = 2;

		inline indent indent(size_t level) const
		{
			return ::html::indent{ *this, level };
		}
	};

	std::ostream& operator<<(std::ostream& os, const indent& ind);

	namespace detail
	{
		std::ostream& format_attribute(std::ostream& os, const format&, const element::attribute& attr);
		void write_content(std::ostream& os, const format& fmt, size_t level, const std::string& in);
		std::ostream& close_element(std::ostream& os, const format& fmt, size_t level, const element& e);
		std::ostream& write(std::ostream& os, const format& fmt, const element& e, size_t level);
	}
}

#endif