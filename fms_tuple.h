// fms_tuple.h - iterable tuples
#pragma once
#include <functional>
#include <tuple>
#include "fms_iterable.h"

namespace fms {

	namespace {
		template <class... Args>
		constexpr auto make_tuple_impl(Args&&... args) {
			return[...args = std::reference_wrapper<Args>{ static_cast<Args&&>(args) }](auto&& fn) mutable -> decltype(auto) {
				return decltype(fn)(fn)(args.type...);
			};
		}
	}
	template<class... Is>
	class tuple {
		decltype(make_tuple_impl<Is...>(std::declval<Is>()...)) tuplefun;
	public:
		// default ctor
		constexpr tuple()
			noexcept ( (std::is_nothrow_default_constructible_v<Is> && ...) ) 
			requires ( (std::is_default_constructible_v<Is> && ...) )
			: tuplfun(make_tuple_impl<Is...>(Is{}...)) 
		{
		}
	
		// explicit ctor
		template <class... Args> 
			requires ((std::is_constructible_v<Is, Args&&> && ...))
		explicit constexpr tuple(Args&&... args)
			noexcept ((std::is_nothrow_constructible_v<Is, Args&&> && ...))
			: tuplefun(make_tuple_impl<Is...>(Is{ static_cast<Args&&>(args) }...))
		{
		}
	};
	/*
	template<class... Is>
	inline auto tuple_bool_all(Is&&... is)
	{
		return [...is = std::forward<Is>(is)]() -> bool {
			return ... && is; // && false;???
		};
	}
	template<class... Is>
	inline auto tuple_bool_any(Is&&... is)
	{
		return[...is = std::forward<Is>(is)]() -> bool {
			return ... || is; // || true;???
		};
	}
	template<class... Is>
	inline auto tuple_star(Is&&... is)
	{
		return [...is = std::forward<Is>(is)]() {
			return std::tuple<decltype(*is)...>(*is...);
		};
	}
	template<class... Is>
	inline auto tuple_incr(Is&&... is)
	{
		return[&...is = std::forward<Is>(is)]() {
			++is, ...;
			//std::tuple<Is...>(++is...); //???why is std::tuple needed
		};
	}
	*/

}