// fms_tuple.h - iterable tuples
// ??? zip
#pragma once
#include <functional>
#include <tuple>
#include <type_traits>
#include "fms_iterable.h"

namespace fms {

	// capture iterator references
	template<class... Is>
	inline constexpr auto capture(Is&&... is) {
		return[&...is = std::forward<Is>(is)](auto&& f) mutable -> decltype(auto)
		{
			return f(is...);
		};
	}

	template<iterable... Is>
	class col {
		std::invoke_result_t<decltype(capture<Is...>), Is&&...> c;
	public:
		col(Is&&... is)
			: c(capture(is...))
		{ }
	};

}
