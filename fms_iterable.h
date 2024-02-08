// fms_iterable.h - iterators with `explicit operator bool() const`
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
//#include <compare>
#include <concepts>
//#include <functional>
#include <iterator>
//#include <limits>
//#include <numeric>
//#include <utility>

namespace fms::iterable {

	template<class I>
	concept forward = requires (I i) {
		std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
		//typename I::iterator_concept;
		typename I::iterator_category;
		//typename I::difference_type;
		typename I::value_type;
		typename I::reference;
		{ !!i } -> std::same_as<bool>;
		{ *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		{ i++ } -> std::same_as<I>;
		{ i.operator==() } -> std::same_as<bool>;
		{ i.begin() } -> std::same_as<I>;
		{ i.end() } -> std::same_as<I>;
	};

	// unsafe pointer iterator
	template<class T>
	class ptr {
		T* p;
	public:
		using iterator_category = std::forward_iterator_tag;
		//using difference_type = ptrdiff_t;
		//using pointer = T*;
		using value_type = std::remove_cv_t<T>;
		using reference = value_type&;

		constexpr ptr(T* p = nullptr)
			: p(p)
		{ }
		constexpr ptr(const ptr&) = default;
		constexpr ptr& operator=(const ptr&) = default;
		constexpr ~ptr() = default;

		constexpr bool operator==(const ptr&) const = default;
		
		constexpr ptr begin() const
		{
			return *this;
		}
		constexpr ptr end() const
		{
			return ptr{};
		}

		constexpr explicit operator bool() const
		{
			return p != nullptr;
		}

		constexpr value_type operator*() const
		{
			return *p;
		}
		constexpr reference operator*()
		{
			return *p;
		}

		constexpr ptr& operator++()
		{
			++p;

			return *this;
		}
		constexpr ptr operator++(int)
		{
			auto tmp = *this;

			operator++();

			return tmp;
		}
	};

#ifdef _DEBUG
	inline int ptr_test()
	{
		{
			static constexpr int i[] = { 0, 1, 2 };
			constexpr auto ii = ptr(i);
			static_assert(ii);
			static_assert(*ii == 0);
			constexpr auto i2(ii);
			static_assert(i2);
			static_assert(ii == i2);
			constexpr auto i3 = i2;
			static_assert(i3);
			static_assert(!(i3 != i2));
		}
		{
			static constexpr int i[] = { 0, 1, 2 };
			constexpr auto ii = ptr(i);
			static_assert(ii);
			static_assert(*ii == 0);
			constexpr auto i2 = ++ptr(ii);
			static_assert(*i2 == 1);
			constexpr auto i3 = ++ptr(i2);
			static_assert(*i3 == 2);
		}
		{
			int i[] = { 0, 1, 2 };
			auto ii = ptr(i);
			*i = 3;
			assert(*i == 3);
			*++ii = 4;
			ii++;
			*ii = 5;
			assert(i[0] == 3);
			assert(i[1] == 4);
			assert(i[2] == 5);
		}

		return 0;
	}
#endif
} // namespace fms::iterable