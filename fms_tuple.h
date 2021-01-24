// fms_tuple.h - iterable tuples
#pragma once
#include <tuple>
#include "fms_iterable.h"

namespace fms {

	template<class... Is>
	inline auto tuple_bool(Is&&... is)
	{
		return[...is = std::forward<Is>(is)]() {
			return std::tuple<decltype(*is)...>(*is...); //???why is std::tuple needed
		};
	}
	template<class... Is>
	inline auto tuple_star(Is&&... is)
	{
		return[...is = std::forward<Is>(is)]() {
			return std::tuple<decltype(*is)...>(*is...); //???why is std::tuple needed
		};
	}
	template<class... Is>
	inline auto tuple_incr(Is&&... is)
	{
		return[&...is = std::forward<Is>(is)]() {
			std::tuple<Is...>(++is...); //???why is std::tuple needed
		};
	}

}