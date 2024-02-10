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
#include <tuple>

namespace fms::iterable {

	// Iterable interval [b, e)
	template<class T>
	class iota {
		T b, e;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = decltype(e - b);
		using pointer = T*;
		using reference = T&;

		iota(T b, T e)
			: b(b), e(e)
		{ }

		// Equality comparable.
		bool operator==(const iota&) const = default;

		// STL friendly
		iota begin() const noexcept
		{
			return *this;
		}
		iota end() const noexcept
		{
			return iota(e, e);
		}

		// Detect end of iterable.
		explicit operator bool() const noexcept
		{
			return b != e;
		}

		value_type operator*() const noexcept
		{
			return b;
		}

		iota& operator++() noexcept
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
		iota operator++(int) noexcept
		{
			auto tmp = *this;

			operator++();

			return tmp;
		}
	};
#ifdef _DEBUG
	inline int iota_test()
	{
		{
			iota i(0, 3);
			assert(i);

			auto i2{ i };
			assert(i2 == i);
			i = i2;
			assert(!(i != i2));

			assert(0 == *i);
			assert(1 == *++i);
			assert(1 == *i++);
			assert(2 == *i);
			assert(!++i);
		}

		return 0;
	}
#endif // _DEBUG

	template<class I>
	concept input_or_output = std::input_or_output_iterator<I>
		&& requires(I i) {
			{ i.operator bool() } -> std::same_as<bool>;
	};

	// Length of iterable.
	template<input_or_output I>
	constexpr size_t count(I i, size_t n = 0)
	{
		if (!i) return n;

		return count(++i, n + 1);
	}
#ifdef _DEBUG
	inline int count_test()
	{
		{
			assert(3 == count(iota(0, 3)));
			assert(5 == count(iota(0, 3), count(iota(3, 5))));
		}

		return 0;
	}
#endif // _DEBUG

	// Last element of iterable.
	template<input_or_output I>
	inline I back(I i)
	{
		I _i = i;

		while (i) {
			_i = i++;
		}

		return _i;
	}
#ifdef _DEBUG
	inline int back_test()
	{
		{
			assert(2 == *back(iota(0, 3)));
		}

		return 0;
	}
#endif // _DEBUG

	template<class I>
	concept input = input_or_output<I>
		&& std::indirectly_readable<I>
		&& std::equality_comparable<I>
	;

	template<class I, class T>
	concept output = input_or_output<I>
		&& std::indirectly_writable<I, T>
		&& requires(I i, T&& t) {
		*i++ = std::forward<T>(t); 
	};

	template<class I>
	concept forward = input<I>
		&& std::derived_from<typename I::iterator_category, std::forward_iterator_tag>
		&& std::incrementable<I>
		;

	// three way lexicographical compare
	template<input_or_output I, input_or_output J> //, typename T = std::common_type_t<typename I::value_type, typename J::value_type>>
	constexpr auto compare(I i, J j)
	{
		while (i and j) {
			const auto cmp = *i++ <=> *j++;
			if (cmp != 0) {
				return cmp;
			}
		}

		return i.operator bool() <=> j.operator bool();
	}
	template<input_or_output I, input_or_output J>
	constexpr bool equal(I i, J j)
	{
		return compare(i, j) == 0;
	}

	template<input I, class J>
	constexpr J copy(I i, J j)
	{
		while (i) {
			*j++ = *i++;
		}

		return j;
	}
#ifdef _DEBUG
	inline int copy_test()
	{
		{
			int j[3];
			copy(iota(0, 3), &j[0]);
			assert(0 == j[0]);
			assert(1 == j[1]);
			assert(2 == j[2]);
		}
		{
			iota i(0, 3);
			int j[3] = { 3 };
			for (const auto& k : i) {
				j[k] = k;
			}
			assert(0 == j[0]);
			assert(1 == j[1]);
			assert(2 == j[2]);
		}

		return 0;
	}
#endif // _DEBUG

	// unsafe pointer iterator
	template<class T>
	class ptr {
		T* p;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

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
		reference operator*() noexcept
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
		{ // op bool, op*, op++, op++(int)
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
#endif // _DEBUG
	
	// Half open interval [b, e)
	template<class I>
	class span {
		I b, e;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename std::iterator_traits<I>::value_type;
		using difference_type = typename std::iterator_traits<I>::difference_type;
		using pointer = typename std::iterator_traits<I>::pointer;
		using reference = typename std::iterator_traits<I>::reference;

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
		reference operator*() noexcept
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
			assert(3 == count(ii));
			span i2(i, 3);
			assert(compare(ii, i2) == 0);
			assert(equal(ii, i2));
		}

		return 0;
	}
#endif // _DEBUG

#if 0
	// End when predicate is true.
	template<forward I, class P>
	class upto {
		I i;
		P p;
	};

	// Iterable of iterables based on separator.
	template<forward I, class T = typename I::value_type>
	class split {
		I i;
		T t;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = I;
		//using difference_type = typename std::iterator_traits<I>::difference_type;
		//using pointer = typename std::iterator_traits<I>::pointer;
		//using reference = typename std::iterator_traits<I>::reference;
		
		split(I i, T t)
			: i(i), t(t)
		{ }

		bool operator==(const split&) const = default;

		split begin() const noexcept
		{
			return *this;
		}
		split end() const noexcept
		{
			return split(i.end(), t);
		}

		explicit operator bool() const noexcept
		{
			return i;
		}
		value_type operator*() const noexcept
		{
			return i;
		}
		split& operator++() noexcept
		{
			if (operator bool()) {
				while (i and *i == t) {
					++i;
				}
				if (i) {
					auto j = i;
					while (j and *j != t) {
						++j;
					}
					i = j;
				}
			}

			return *this;
		}
	};
#endif // 0
} // namespace fms::iterable
