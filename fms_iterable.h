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
		std::is_base_of_v<std::forward_iterator_tag, typename I::iterator_category>;
		typename I::iterator_category;
		typename I::value_type;
		{ i == i } -> std::same_as<bool>;
		{ i.begin() } -> std::same_as<I>;
		{ i.end() } -> std::same_as<I>;
		{ !!i } -> std::same_as<bool>;
		{ *i  } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		{ i++ } -> std::same_as<I>;
	};

	// three way lexicographical compare
	template<forward I, forward J,
		typename T = std::common_type_t<typename I::value_type, typename J::value_type>>
	constexpr auto compare(I i, J j)
	{
		while (i and j) {
			const auto cmp = T(*i++) <=> T(*j++);
			if (cmp != 0) {
				return cmp;
			}
		}

		return T(!!i) <=> T(!!j);
	}
	template<forward I, forward J>
	constexpr bool equal(I i, J j)
	{
		return compare(i, j) == 0;
	}

	template<class I>
	inline size_t length(I i, size_t n = 0)
	{
		while (i++) {
			++n;
		}

		return n;
	}

	// unsafe pointer iterator
	template<class T>
	class ptr {
		T* p;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::remove_cv_t<T>;

		ptr(T* p = nullptr) noexcept
			: p(p)
		{ }
		
		// Same pointer.
		bool operator==(const ptr&) const = default;
		
		ptr begin() const noexcept
		{
			return *this;
		}
		ptr end() const noexcept
		{
			return ptr{};
		}

		explicit operator bool() const noexcept
		{
			return p != nullptr;
		}

		value_type operator*() const noexcept
		{
			return *p;
		}

		ptr& operator++() noexcept
		{
			++p;

			return *this;
		}
		ptr operator++(int) noexcept
		{
			auto tmp = *this;

			operator++();

			return tmp;
		}
	};

#ifdef _DEBUG
	inline int ptr_test()
	{
		{ // constructors, equality
			static int i[] = { 0, 1, 2 };
			auto ii = ptr(i);
			assert(ii);
			assert(*ii == 0);
			auto i2(ii);
			assert(i2);
			assert(ii == i2);
			auto i3 = i2;
			assert(i3);
			assert(!(i3 != i2));
		}
		{ // begin, end
			int i[] = { 0, 1, 2 };
			auto ii = ptr(i);
			assert(ii == ii.begin());
			assert(!ii.end());
			int j = 0;
			for (const auto& k : ii) {
				assert(k == i[j++]);
				if (j == 3) {
					break;
				}
			}
		}
		{
			// op bool, op*, op++, op++(int)
			int i[] = { 0, 1, 2 };
			auto ii = ptr(i);
			assert(ii);
			assert(*ii == 0);
			++ii;
			assert(*ii == 1);
			assert(*ii++ == 1);
			assert(*ii == 2);
		}

		return 0;
	}
#endif

	// STL interval [b, e)
	template<class I>
	class span {
		I b, e;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename std::iterator_traits<I>::value_type;

		span(I b, I e)
			: b(b), e(e)
		{ }
		span(I b, size_t n)
			: b(b), e(std::next(b, n))
		{ }
		template<size_t N>
		span(value_type(&a)[N])
			: b(a), e(a + N)
		{ }

		bool operator==(const span& s) const
		{
			return b == s.b && e == s.e;
		}

		span begin() const noexcept
		{
			return *this;
		}
		span end() const noexcept
		{
			return span(e, e);
		}

		explicit operator bool() const noexcept
		{
			return b != e;
		}
		value_type operator*() const noexcept
		{
			return *b;
		}
		span& operator++() noexcept
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
		span operator++(int) noexcept
		{
			auto tmp = *this;

			operator++();

			return tmp;
		}
	};

	template<class I>
	inline span<I> take(size_t n, I i)
	{
		return span(i, std::next(i, n));
	}
	template<class I>
	inline I drop(size_t n, I i)
	{
		return std::next(i, n);
	}
#ifdef _DEBUG
	inline int span_test()
	{
		{
			int i[] = { 0, 1, 2 };
			span ii(i, i + 3);
			assert(ii);
			assert(!(ii != ii));
			assert(3 == length(ii));
			span i2(i, 3);
			assert(compare(ii, i2) == 0);
			assert(equal(ii, i2));
		}

		return 0;
	}
#endif // _DEBUG

} // namespace fms::iterable