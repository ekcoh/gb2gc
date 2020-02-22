// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifdef _MSC_VER
#pragma once    // Improves build time on MSVC
#endif

#ifndef GB2GC_DOCUMENT_H
#define GB2GC_DOCUMENT_H

#include <string>
#include <functional>
#include <ostream>
#include <sstream>
#include <vector>

namespace gb2gc {
   
struct format;

class element
{
public:
    using element_container = std::vector<element>;
    using element_iterator = typename element_container::iterator;
    using const_element_iterator = typename element_container::const_iterator;

    using attribute = std::pair<std::string, std::string>;
    using attribute_container = std::vector<attribute>;
    using attribute_iterator = typename attribute_container::iterator;

    using convertible = std::function<void(
        std::ostream&, // os
        const format&, // fmt
        size_t)>;      // level

    enum class content_type
    {
        node,
        string,
        convertible
    };

    explicit element(const std::string& name);
    ~element() noexcept = default;
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

    const_element_iterator find_child(const std::string& value);

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
    std::string            name_;
    std::string            value_;
    std::string            comment_;
    convertible            convertible_value_;
    std::vector<attribute> attributes_;
    std::vector<element>   children_;
};

struct indent
{
    const format& fmt;
    size_t        level;
};

std::ostream& operator<<(std::ostream& os, const indent& ind);

struct format
{
    bool     strip_comments = false;
    unsigned indentation    = 2;

    inline ::gb2gc::indent indent(size_t level) const
    {
        return ::gb2gc::indent{ *this, level };
    }
};

std::ostream& write_dom(std::ostream& os, const element& e,
    const format& fmt = format(), size_t level = 0);

} // namespace gb2gc

#endif // GB2GC_DOCUMENT_H