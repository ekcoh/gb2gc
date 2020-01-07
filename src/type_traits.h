#ifndef GB2GC_TYPE_TRAITS_H
#define GB2GC_TYPE_TRAITS_H

#include <type_traits>

namespace gb2gc
{
	template<typename T, typename = void>
	struct is_iterator
	{
		static constexpr bool value = false;
	};

	template<typename T>
	struct is_iterator<T, typename std::enable_if<
        !std::is_same<typename std::iterator_traits<T>::value_type, void>::value
    >::type>
	{
		static constexpr bool value = true;
	};

}

#endif // GB2GC_HTML_TYPE_TRAITS_H