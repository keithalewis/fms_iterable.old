// fms_iterable.h - iterators with `explicit operator bool() const`
//#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <compare>
#include <concepts>
//#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
//#include <utility>

// to make tests return false instead of abort
// #define assert(e) if (!(e)) { return false; }

namespace fms {

	template<class I>
	concept iterable = requires (I i) {
		//std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
		//typename I::iterator_concept;
		//typename I::iterator_category;
		//typename I::difference_type;
		typename I::value_type;
		//typename I::reference;
		{ !!i } -> std::same_as<bool>;
		{  *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		//{ i.operator<=>(i) };
		//{ i.end() } -> std::same_as<I>;
	};

	// "All happy iterables begin alike..."
	template<iterable I>
	constexpr I begin(const I& i)
	{
		return i;
	}
	// "...but each iterable ends after its own fashion."
	template<iterable I>
	constexpr auto end(const I& i) -> decltype(i.end())
	{
		return i.end();
	}

	// three way lexicographical compare
	template<iterable I, iterable J>
	constexpr auto compare(I i, J j)
	{
		for (; i and j; ++i, ++j) {
			auto cmp = *i <=> *j;
			if (cmp != 0) {
				return cmp;
			}
		}

		using T = std::common_type_t<typename I::value_type, typename J::value_type>;

		return T(!!i) <=> T(!!j);
	}
	// all values are equal
	template<iterable I, iterable J>
	constexpr bool equal(I i, J j)
	{
		return compare(i, j) == 0;
	}

	// STL range [b, e)
	template<class I>
	class span {
		I b, e;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		//using iterator_concept = typename I::iterator_concept;
		//using iterator_category = typename I::iterator_category;
		//using difference_type = typename std::iterator_traits<I>::difference_type;
		using value_type = typename std::iterator_traits<I>::value_type;
		using reference = typename std::iterator_traits<I>::reference;

		constexpr span()
		{ }
		constexpr span(I b, I e)
			: b(b), e(e)
		{ }
		constexpr span(const span&) = default;
		constexpr span& operator=(const span&) = default;
		constexpr ~span() = default;

		constexpr bool operator==(const span&) const = default;
		constexpr auto end() const
		{
			return span(e, e);
		}

		constexpr explicit operator bool() const
		{
			return b != e;
		}
		constexpr value_type operator*() const
		{
			return *b;
		}
		constexpr span& operator++()
		{
			if (b != e) {
				++b;
			}

			return *this;
		}
#ifdef _DEBUG

		static int test()
		{
			using T = value_type;
			static constexpr T t[] = { 0, 1, 2 };
			{
				constexpr auto i = span(t, t + 3);
				static_assert(i);
				constexpr auto i2(i);
				static_assert(i2);
				static_assert(i2 == i);
				constexpr auto i3 = i2;
				static_assert(i3);
				static_assert(!(i3 != i2));
			}
			{
				auto i = span(t, t + 3);
				assert(i);
				assert(*i == t[0]);
				++i;
				assert(i);
				assert(*i == t[1]);
				assert(*++i == t[2]);
				++i;
				assert(!i);
			}
			{
				span i(t, t + 3);
				assert(i);
				assert(*i == 0);
				++i;
				assert(i);
				assert(*i == 1);
				++i;
				assert(i);
				assert(*i == 2);
				++i;
				assert(!i);
			}
			{
				T u(0);
				for (auto i : span(t, t + 3)) {
					assert(i == u);
					++u;
				}
			}

			return true;
		}

#endif // _DEBUG
	};

	template<class T>
	class constant {
		T t;
	public:
		using iterator_concept = std::random_access_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = std::remove_cv_t<T>;
		using reference = T&;

		constexpr constant(T t = std::numeric_limits<T>::max())
			: t(t)
		{ }
		constexpr constant(const constant&) = default;
		constexpr constant& operator=(const constant&) = default;
		constexpr ~constant() = default;

		auto operator<=>(const constant&) const = default;
		
		explicit operator bool() const
		{
			return true;
		}
		constant end() const
		{
			return constant{};
		}
		value_type operator*() const
		{
			return t;
		}
		reference operator*()
		{
			return t;
		}
		constant& operator++()
		{
			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				constant i;
				assert(i);
				auto i2{ i };
				assert(i == i2);
				i = i2;
				assert(!(i2 != i));

				assert(*i == std::numeric_limits<T>::max());
				++i;
				assert(i);
				assert(*i == std::numeric_limits<T>::max());
			}
			{
				T u(0);
				int n = 3;
				for (auto i : constant(u)) {
					assert(i == u);
					--n;
					if (n == 0) {
						break;
					}
				}
			}

			return 0;
		}
#endif // _DEBUG
	};

	// [b, b + 1, ..., e)
	template<class T>
	class iota {
		T b, e;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = std::remove_cv_t<T>;
		using reference = T&;

		constexpr iota(T b = 0, T e = std::numeric_limits<T>::max())
			: b(b), e(e)
		{ }
		constexpr iota(const iota&) = default;
		constexpr iota& operator=(const iota&) = default;
		constexpr ~iota() = default;

		constexpr bool operator==(const iota&) const = default;	
		auto end() const
		{
			return iota(e, e);
		}

		constexpr explicit operator bool() const
		{
			return b < e;
		}
		constexpr value_type operator*() const
		{
			return b;
		}
		constexpr iota& operator++()
		{
			if (b < e) {
				++b;
			}

			return *this;
		}
#ifdef _DEBUG

		static int test()
		{
			{
				assert(equal(iota(0, 3), iota(0, 3)));
				assert(!equal(iota(0, 2), iota(0, 3)));
			}
			{
				assert(compare(iota(0, 3), iota(0, 3)) == 0);
				assert(compare(iota(0, 2), iota(0, 3)) < 0);
				assert(compare(iota(0, 3), iota(0, 2)) > 0);
			}
			{
				assert(compare(iota(0, 3), iota<float>(0, 3)) == 0);
				assert(compare(iota<double>(0, 2), iota(0, 3)) < 0);
				assert(compare(iota<float>(0, 3), iota<double>(0, 2)) > 0);
			}
			{ // ctors, <=>
				constexpr auto i = iota<T>{};
				static_assert(i);
				constexpr auto i2(i);
				static_assert(i2);
				static_assert(i2 == i);
				constexpr auto i3 = i2;
				assert(i3);
				assert(!(i3 != i2));
			}
			{
				iota<T> i(1);
				assert(i);
				assert(*i == 1);
				++i;
				assert(i);
				assert(*i == 2);
			}
			{
				iota<T> i(0, 3);
				assert(i);
				assert(*i == 0);
				++i;
				assert(i);
				assert(*i == 1);
				++i;
				assert(i);
				assert(*i == 2);
				++i;
				assert(!i);
			}
			{
				T t(0);
				for (auto i : iota<T>(0, 3)) {
					assert(i == t);
					++t;
				}
				assert(t == 3);
			}

			return true;
		}

#endif // _DEBUG
	};

	// create an iterable from a random access array
	// nullptr gives 'empty' iterator
	template<class T>
	class array {
	protected:
		size_t n;
		T* a;
	public:
		using iterator_concept = std::contiguous_iterator_tag;
		using iterator_category = std::bidirectional_iterator_tag;;
		using difference_type = ptrdiff_t;
		using value_type = std::remove_cv_t<T>;
		using reference = T&;

		constexpr array(size_t n = 0, T* a = nullptr)
			: n(n), a(a)
		{ }
		template<size_t N>
		constexpr array(T(&a)[N])
			: array(N, a)
		{ }
		constexpr array(const array&) = default;
		constexpr array& operator=(const array&) = default;
		constexpr ~array() = default;

		constexpr array end() const
		{
			return array(0, a + n);
		}
		constexpr bool operator==(const array&) const = default;

		constexpr explicit operator bool() const
		{
			return n != 0;
		}

		constexpr value_type operator*() const
		{
			return *a;
		}
		constexpr reference operator*()
		{
			return *a;
		}

		constexpr value_type operator[](difference_type n) const
		{
			return a[n];
		}
		constexpr reference operator[](difference_type n)
		{
			return a[n];
		}

		constexpr array& operator++()
		{
			if (n) {
				--n;
				++a;
			}

			return *this;
		}
		constexpr array operator++(int)
		{
			auto a_ = *this;

			operator++();

			return a_;
		}
		constexpr array& operator--()
		{
			if (n) {
				++n;
				--a;
			}

			return *this;
		}
		constexpr array operator--(int)
		{
			auto a_ = *this;

			operator--();

			return a_;
		}

#ifdef _DEBUG
#include <numeric>

		static int test()
		{
			using fms::end;
			{
				T t[] = { 1,2,3 };
				array<T> a(t);
				assert(a);
				auto a2(a);
				assert(a2);
				assert(a2 == a);
				a = a2;
				assert(a);
				assert(!(a != a2));
			}
			{
				T t[] = { 1,2,3 };
				array<T> a(t);
				assert(*a == t[0]);

				++a;
				assert(a);
				assert(*a == t[1]);

				assert(*a-- == t[1]);
				assert(*a == t[0]);

				assert(*a++ == t[0]);
				assert(*a == t[1]);
			}
			{
				T t[] = { 1,2,3 };
				array<T> a(t);
				assert(equal(a, iota(1, 4)));
				assert(compare(a, iota(1, 4)) == 0);
			}
			{
				T t[] = { 1,2,3 };
				array<T> a(t);
				assert(6 == std::accumulate(begin(a), end(a), T(0)));
			}
			{
				array<T> e; // empty
				assert(!e);
				assert(e == begin(e));
				assert(e == end(e));
				assert(equal(e, e));
				assert(compare(e, e) == 0);
			}

			return true;
		}

#endif // _DEBUG
	};

} // namespace fms

#if 0
#include "fms_function.h"
#include "fms_numeric.h"

namespace fms {

#pragma region null


#pragma endregion null


#pragma region array

	template<class T>
	class array : public pointer<T> {
		size_t n;
	public:
		using iterator_concept = typename std::iterator_traits<T*>::iterator_concept;
		using iterator_category = typename std::iterator_traits<T*>::iterator_category;
		using difference_type = typename std::iterator_traits<T*>::difference_type;
		using reference = T&;
		using value_type = T;

		array()
			: pointer<T>(nullptr), n(0)
		{ }
		array(size_t n, const pointer<T>& a)
			: pointer<T>(a), n(n)
		{ }
		array(size_t n, T* a)
			: pointer<T>(a), n(n)
		{ }
		template<size_t N>
		array(T(&a)[N])
			: array(N, a)
		{ }
		array(const array&) = default;
		array& operator=(const array&) = default;
		~array()
		{ }

		// remaining size
		size_t size() const
		{
			return n;
		}

		auto operator<=>(const array&) const = default;

		explicit operator bool() const
		{
			return n != 0;
		}
		array end() const
		{
			return array(0, pointer<T>::p + n);
		}

		using pointer<T>::operator*;

		array& operator++()
		{
			if (*this) {
				--n;
				pointer<T>::operator++();
			}

			return *this;
		}

		//!!! overide operator+=, etc
	};

#ifdef _DEBUG

	template<class T>
	inline bool test_array()
	{
		{
			array<T> a;
			auto a2(a);
			a = a2;
			assert(a == a2);
			assert(!(a2 != a));
			assert(a.size() == 0);
			assert(!a);
			++a;
			assert(a.size() == 0);
			assert(!a);
		}
		{
			T a_[] = { 1,2,3 };
			array<T> a(a_);
			auto a2(a);
			a = a2;
			assert(a == a2);
			assert(!(a2 != a));

			assert(a.size() == 3);
			assert(a);
			assert(*a == 1);

			++a;
			assert(a.size() == 2);
			assert(a);
			assert(*a == 2);

			++a;
			assert(a.size() == 1);
			assert(a);
			assert(*a == 3);

			++a;
			assert(a.size() == 0);
			assert(!a);

			++a;
			assert(a.size() == 0);
			assert(!a);
		}
		{
			T a_[] = { 1,2,3 };
			array<T> a(a_);

			auto a0 = *a;
			for (const auto& ai : a) {
				assert(ai == a0);
				++a0;
			}
		}
		{
			T a_[] = { 1,2,3 };
			array<T> a(3, pointer(a_));
			T s = 0;
			for (auto i : a) {
				s += i;
			}
			assert(6 == s);
		}

		return true;
	}


#endif // _DEBUG

#pragma endregion array

#pragma region list

	// create from initializer_list
	template<typename T>
	class list : public array<T> {
		std::vector<T> a;
	public:
		using iterator_concept = typename array<T>::iterator_concept;
		using iterator_category = typename array<T>::iterator_category;
		using difference_type = typename array<T>::difference_type;
		using reference = T&;
		using value_type = T;

		list()
			: array<T>(0, nullptr)
		{ }
		list(std::initializer_list<T> as)
			: array<T>(as.size(), nullptr), a(as)
		{
			pointer<T>::p = a.data();
		}
		list(const list&) = default;
		list& operator=(const list&) = default;
		~list()
		{ }
		/*
		using array<T>::operator<=>;
		using array<T>::end;
		using array<T>::operator bool;
		using pointer<T>::operator*;
		using array<T>::operator++;
		*/
		//??? covariant return type not available
		list& operator++()
		{
			array<T>::operator++();

			return *this;
		}
	};

#ifdef _DEBUG

	inline bool test_list()
	{
		{
			list l({ 1,2,3 });
			assert(l);
			auto l2(l);
			assert(l2);
			assert(l2 == l);
			//assert(equal(l, l2));
			l = l2;
			assert(!(l != l2));
			//assert(size(l) == 3);

			assert(*l == 1);
			++l;
			assert(*l == 2);
			++l;
			assert(*l == 3);
			++l;
			assert(!l);
		}
		{
			list l({ 1,2,3 });
			assert(size(l) == 3);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion list


#pragma region take

	// take first (>0) or last (<0) n items
	// same as done(countdown(n), i)
	template<iterable I>
	class take : public I {
		size_t n;
	public:
		using iterator_concept = typename I::iterator_concept; 
		using iterator_category = typename I::iterator_category;
		using difference_type = typename I::difference_type;
		using value_type = typename I::value_type;
		using reference = typename I::reference;

		take()
			: I{}, n(0)
		{ }
		take(long n, const I& i)
			: I(i), n(n)
		{ }

		// remaining size
		size_t size() const
		{
			return n;
		}

		auto operator<=>(const take&) const = default;

		explicit operator bool() const
		{
			return n != 0;
		}
		take end() const
		{
			return take(0, skip(n, *this));
		}

		using I::operator*;

		take& operator++()
		{
			if (*this) {
				--n;
				I::operator++();
			}

			return *this;
		}
		take& operator++(int)
		{
			if (*this) {
				take t_ = *this;
				--n;
				I::operator++();
				return t_;
			}

			return *this;
		}
		//if constexpr (...) operator++(int) ...
	};


#ifdef _DEBUG

	template<typename T>
	inline bool test_take()
	{
		{
			auto t = take(2, iota<T>{});
			auto t2(t);
			t = t2;
			assert(t);
			assert(length(t) == 2);
			assert(*t == 0);
			++t;

			assert(t);
			assert(length(t) == 1);
			assert(*t == 1);
			++t;

			assert(!t);
			assert(size(t) == 0);
		}
		{
			auto t3 = take(3, iota<int>(0));
			size_t n = 0;
			for (const auto& t : t3) {
				++n;
			}
			assert(n == 3);
			assert(3 == size(t3));
		}
		{
			auto t0 = take(0, iota<int>(0));
			assert(!t0);
			assert(0 == length(t0));
			assert(equal(t0, pointer<int>{}));
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion take

#pragma region constant

	template<typename T>
	class constant {
		T t;
	public:
		using iterator_concept = typename std::iterator_traits<T*>::iterator_concept;
		using iterator_category = typename std::iterator_traits<T*>::iterator_category;
		using difference_type = typename std::iterator_traits<T*>::difference_type;
		using reference = T&;
		using value_type = T;

		constant()
			: t(0)
		{ }
		constant(const T& t)
			: t(t)
		{ }
		constant(const constant&) = default;
		constant& operator=(const constant&) = default;
		~constant()
		{ }

		auto operator<=>(const constant&) const = default;

		explicit operator bool() const
		{
			return true;
		}
		constant end() const
		{
			return constant(std::numeric_limits<T>::max()); //??? what else
		}

		value_type operator*() const
		{
			return t;
		}
		reference operator*()
		{
			return t;
		}

		value_type operator[](difference_type n) const
		{
			return t;
		}
		reference operator[](difference_type n)
		{
			return t;
		}

		constant& operator++()
		{
			return *this;
		}
		constant operator++(int)
		{
			return *this;
		}
		constant& operator--()
		{
			return *this;
		}
		constant operator--(int)
		{
			return *this;
		}

		constant& operator+=(difference_type n)
		{
			return *this;
		}
		constant& operator-=(difference_type n)
		{
			return *this;
		}
	};

	// length 1 iterable {t}
	template<class T>
	inline auto unit(const T& t)
	{
		return take(1, constant(t));
	}

} // namespace fms

template<typename T>
inline auto operator+(fms::constant<T> c, typename fms::constant<T>::difference_type n)
{
	return c;
}
template<typename T>
inline auto operator+(typename fms::constant<T>::difference_type n, fms::constant<T> c)
{
	return c;
}
template<typename T>
inline auto operator-(fms::constant<T> c, typename fms::constant<T>::difference_type n)
{
	return c;
}

namespace fms {
#ifdef _DEBUG
#include <cassert>

	template<typename T>
	inline bool test_constant()
	{
		{
			constant<T> c;
			auto c2 = c;
			c = c2;
			assert(c == c2);
			assert(c2 == c);
			assert(c);
			++c;
			assert(c);
		}
		{
			constant<T> c(1);
			auto c2 = c;
			c = c2;
			assert(c == c2);
			assert(c2 == c);
			assert(c);
			assert(*c == 1);
			assert(1 == *c);
			++c;
			assert(c);
			assert(*c == 1);
			assert(1 == *c);
			assert(begin(c) != end(c));
		}
		{
			auto u = unit(3);
			auto u2(u);
			assert(u2);
			u = u2;
			assert(equal(u, u2));
			assert(length(u) == 1);
			assert(*u == 3);
			*u = 4;
			assert(4 == *u);
			++u;
			assert(!u);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion constant


#pragma region null
	
	// zero value ends iterable
	template<iterable I>
	struct null : public I {
		null()
		{ }
		null(const I& n)
			: I(n)
		{ }

		explicit operator bool() const
		{
			return I::operator*() != 0;
		}
	};

#ifdef _DEBUG

	inline bool test_null()
	{
		auto test_abc = [](auto n)
		{
			static_assert(std::is_same_v<typename decltype(n)::value_type, const char>);
			assert(n);
			assert(*n == 'a');

			auto n2(n);
			n = n2;
			assert(n == n2);
			assert(!(n2 != n));
			assert(n <= n2);
			assert(!(n < n2));
			assert(n >= n2);
			assert(!(n > n2));

			++n;
			assert(n);
			assert(*n == 'b');

			++n;
			assert(n);
			assert(*n == 'c');

			++n;
			assert(!n);
		};
		{
			const char* s = "abc";
			auto n = null(pointer(s));
			test_abc(n);
		}
		{
			test_abc(null(pointer("abc")));
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion null


#pragma region done

	//??? done using bool(const I&)
	template<iterable I>
	class done : public I {
		I e;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		done(const I& i, const I& e)
			: I(i), e(e)
		{ }
		auto operator<=>(const done&) const = default;

		explicit operator bool() const
		{
			return !I::operator==(e);
		}
		done end() const
		{
			return done(e, e);
		}

		value_type operator*() const
		{
			return I::operator*();
		}
		done& operator++()
		{
			if (this) {
				I::operator++();
			}

			return *this;
		}
	};

#pragma endregion done

#pragma region scan

	//??? use done
	// return {{i}, {i,++i}, ... {i, ++i, , ..., i.end()}}
	template<iterable I/*, class N = void**/>
	class scan {
		I i, e;
		//N next; //?? std::function<void(const I&)>
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = done<I>;
		
		scan(const I& i)
			: i(i), e(i) //, data(nullptr)
		{
			++e;
		}

		auto operator<=>(const scan&) const = default;

		explicit operator bool() const
		{
			return e != i.end();
		}
		scan end() const
		{
			return scan(e, e);
		}
		value_type operator*() const
		{
			return done<I>(i, e);
		}
		scan& operator++()
		{
			++e;

			return *this;
		}
	};

#ifdef _DEBUG

	inline bool test_scan()
	{
		{
			scan s(take(3, iota(0)));
			assert(s);
			auto s2(s);
			assert(s2);
			assert(s == s2);
			s = s2;
			assert(s);
			assert(!(s2 != s));
			for (int n = 1; n < 3; ++n, ++s) {
				assert(equal(*s, take(n, iota(0))));
			}
			assert(!s);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion scan

#pragma region when

	// filter iterable by predicate
	template<iterable I, class P>
	class when : public I {
		std::function<bool(const I&)> p;
		// increment to next iterator satisfying p
		void next()
		{
			while (I::operator bool() and !p(*this)) {
				I::operator++();
			}
		}
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		when(const P& p, const I& i)
			: I(i), p(p)
		{
			next();
		}

		auto operator<=>(const when& w) const
		{
			return I::operator<=>(w);
		}

		using I::operator bool;
		when end() const
		{
			return when(p, I::end());
		}
		when& operator++()
		{
			I::operator++();
			next();

			return *this;
		}
	};

#ifdef _DEBUG

	inline bool test_when()
	{
		{
			auto even = [](const auto& pi) { return *pi % 2 == 0;  };
			when e(even, iota(0));
			assert(e);
			auto e2(e);
			assert(e2);
			assert(e2 == e);
			e = e2;
			assert(e);
			assert(!(e != e2));
			assert(*e == 0);
			++e;
			assert(*e == 2);
			assert(*++e == 4);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion when

#pragma region mask

	template<iterable I, iterable M>
	class mask : public I {
		M m;
		void next()
		{
			while (*this and m and !*m) {
				I::operator++();
				++m;
			}
			//??? check *this and m both true or both false
		}
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		mask(const I& i, const M& m)
			: I(i), m(m)
		{
			next();
		}

		auto operator<=>(const mask&) const = default;

		mask& operator++()
		{
			I::operator++();
			++m;
			next();

			return *this;
		}
	};

#ifdef _DEBUG

	inline bool test_mask()
	{
		{
			auto i = take(4, iota(0));
			int m_[] = { 0, 1, 1, 0 };
			mask m(i, array(m_));
			assert(m);
			auto m2(m);
			assert(m2);
			m = m2;
			assert(m);
			assert(m == m2);
			assert(!(m2 != m));
			
			assert(*m == 1);
			++m;
			assert(*m == 2);
			++m;
			assert(!m);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion mask;

#pragma region cycle

	template<iterable I>
	class cycle : I {
		I i;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;
		cycle(const I& i)
			: I(i), i(i)
		{ }
		explicit operator bool() const
		{
			return true;
		}
		cycle end() const
		{
			return cycle(i.end()); // I::operator==(end()) always false
		}
		using I::operator*;
		cycle& operator++()
		{
			I::operator++();
			if (!I::operator bool()) {
				I::operator=(i);
			}

			return *this;
		}
	};

#pragma endregion cycle

#pragma region until

	// stop when end or p(i) is true
	template<iterable I, class T = typename I::value_type>
	class until : public I {
		std::function<bool(const I&)> p;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		until()
		{ }
		until(const I& i)
			: until([](const I&) { return false; }, i)
		{ }
		template<class P>
		until(P p, const I& i)
			: I(i), p(std::move(p))
		{ }
		until(const until&) = default;
		until& operator=(const until&) = default;
		~until()
		{ }

		explicit operator bool() const
		{
			return !!I(*this) and !p(*this);
		}
		until end() const
		{
			return until(p, I::end());
		}
		value_type operator*() const
		{
			return I::operator*();
		}
		until& operator++()
		{
			if (*this) {
				I::operator++();
			}

			return *this;
		}

		template<class R>
		auto is(const relation<I,R>& r)
		{
			return [r](const I& i) { return until(r, i);  };
		}
	};
#ifdef _DEBUG

	inline bool test_until()
	{
		{
			iota<int> i;
			//auto u = until([](auto i) { return *i == 3; }, i);
			//std::function<bool(const int*)> f = [](o)
			until u([](const auto& i) { return *i == 3; }, i);
			assert(length(u) == 3);
			assert(u);
			auto u2(u);
			assert(u2);
			u = u2;
			assert(u);
			assert(*u == 0);
			++u;
			assert(*u == 1);
			++u;
			assert(*u == 2);
			++u;
			assert(!u);
		}
		{
			iota<int> i;
			auto eq3 = relation<decltype(i), std::equal_to<int>>(3);
			auto u = until(eq3, i);// .is(eq(3), ;
			assert(u);
			auto u2(u);
			assert(u2);
			u = u2;
			assert(u);
			assert(*u == 0);
			++u;
			assert(*u == 1);
			++u;
			assert(*u == 2);
			++u;
			assert(!u);
		}
		{
			/*
			iota<int> i;
			auto eq3 = relation<decltype(i), std::equal_to<int>>(3);
			auto u0 = until(i);
			auto u = u0.is(eq3);
			assert(u);
			auto u2(u);
			assert(u2);
			u = u2;
			assert(u);
			assert(*u == 0);
			++u;
			assert(*u == 1);
			++u;
			assert(*u == 2);
			++u;
			assert(!u);
			*/
		}

		return true;
	}

#endif // _DEBUG

	// True when n consecutive values less or equal to m*epsilon seen
	// If m < 1 use epsilon/m. E.g., m = sqrt(epsilon)
	template<iterable I, class T = typename I::value_type>
	inline auto epsilon(const I& i, T m = 1, unsigned n = 1)
	{
		if (n == 0) {
			n = 1;
		}
		T eps = (m > 1 ? m : 1/m)*std::numeric_limits<T>::epsilon();
		auto done = [eps, n](const I& i) {
			static unsigned n_ = n;

			if (std::abs(*i) <= eps) {
				if (--n_ == 0) {
					return true;
				}
			}
			else {
				n_ = n; // restart count
			}

			return false;
		};

		return until(done, i);
	}

#ifdef _DEBUG
#include "fms_function.h"
	template<class T>
	inline bool test_epsilon()
	{
		{
			auto p = power(1/T(2));
			auto e = epsilon(p);
			size_t n = length(e);
			assert(1 + n == std::numeric_limits<T>::digits);
		}
		{
			assert(1 + length(epsilon(power(1 / T(2)))) == std::numeric_limits<T>::digits);
		}

		return true;
	}

#endif // _DEBUG


#pragma endregion until

/*
#pragma region done

	template<iterable I>
	inline auto done(const I& i, const I& e)
	{
		return until([e](const I& i) { return i == e; }, i);
	}

#ifdef _DEBUG

	inline bool test_sentinal()
	{
		{

		}

		return true;
	}

#endif // _DEBUG

#pragma endregion done
*/


#pragma region apply

	// apply function to sequence
	template<class F, iterable I> //,
	class apply {
		using T = typename I::value_type;
		using U = std::invoke_result_t<F, T>;
		F f;
		I i;
	public:
		using iterator_concept = typename I::iterator_concept;
		using iterator_category = typename I::iterator_category;
		using value_type = U;

		apply()
		{ }
		apply(const F& f, const I& i)
			: f(f), i(i)
		{ }
		apply(const apply&) = default;
		apply& operator=(const apply&) = default;
		~apply()
		{ }

		bool operator==(const apply& a) const
		{
			return i == a.i; // f == a.f must be true;
		}
		apply end() const
		{
			return apply(f, i.end());
		}

		explicit operator bool() const
		{
			return !!i;
		}
		value_type operator*() const
		{
			return f(*i);
		}
		apply& operator++()
		{
			++i;

			return *this;
		}
	};
#ifdef _DEBUG

	inline bool test_apply()
	{
		{
			iota<int> i;
			apply a(std::negate<int>{}, i);
			auto a2(a);
			a = a2;
			assert(a == a2);
			assert(!(a != a2)); 
			assert(a);
			assert(*a == 0);
			++a;
			assert(*a == -1);
		}
		{
			iota<int> i;
			apply a([](int i) { return -i; }, i);
			auto a2(a);
			a = a2;
			assert(a == a2);
			assert(!(a != a2));
			assert(a);
			assert(*a == 0);
			++a;
			assert(*a == -1);
		}
		{
			apply a(std::negate<int>{}, iota<int>{});
			apply b([](int i) { return -i; }, iota<int>{});
			for (int i : {1, 2, 3}) {
				assert(*++a == -i); 
				assert(*++b == -i);
			}
		}
		{
			apply a(std::negate<int>{}, iota<int>{});
			int i = 0;
			for (auto ai = begin(a); ai != end(a); ++ai) {
				assert(*ai == -i);
				++i;
				if (i == 3)
					break;
			}
		}
		{
			apply a(std::negate<int>{}, iota<int>{});
			int i = 0;
			for (auto ai : a) {
				assert(ai == -i);
				++i;
				if (i == 3)
					break;
			}
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion apply

#pragma region binop

	// apply binary operator when both values exist
	//??? what if length(i) != length(j)
	template<class Op, iterable I, iterable J>
	class binop {
		Op op;
		I i;
		J j;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::invoke_result_t<Op, typename I::value_type, typename J::value_type>;

		binop()
		{ }
		binop(const Op& op, const I& i, const J& j)
			: op(op), i(i), j(j)
		{ }
		binop(const binop&) = default;
		binop& operator=(const binop&) = default;
		~binop()
		{ }

		bool operator==(const binop& b) const
		{
			return i == b.i and j == b.j; // op == b.op must be true
		}

		explicit operator bool() const
		{
			return i and j;
		}
		value_type operator*() const
		{
			return op(*i, *j);
		}
		binop& operator++()
		{
			++i;
			++j;

			return *this;
		}
	};
	template<iterable I, iterable J>
	inline auto eq(const I& i, const J& j)
	{
		return binop(std::equal_to{}, i, j);
	}
#define ITERABLE_ARITHMETIC(a,b,c)        \
	template<iterable I, iterable J>      \
	inline auto b(const I& i, const J& j) \
	{ return binop(c{}, i, j); }

FMS_ARITHMETIC_OPS(ITERABLE_ARITHMETIC)
#undef ITERABLE_ARITHMETIC
// ne, lt, le, ge, gt
// logical_and, ..., bit_and, ...
	
#ifdef _DEBUG

	template<class T>
	inline bool test_binop()
	{
		using c = constant<T>;
		{
			T i[] = { 1,2,3 };
			auto a = array(i);
			auto bo = binop(std::equal_to{}, a, a);
			auto bo2(bo);
			bo = bo2;
			assert(bo);
			assert(size(bo) == 3);
			assert(all(bo));
		}
		{
			T i[] = { 1,2,3 };
			auto a = array(i);
			auto bo = eq(a, a);
			auto bo2(bo);
			bo = bo2;
			assert(bo);
			assert(size(bo) == 3);
			assert(all(bo));
		}
		{
			T i[] = { 1,2,3 };
			auto a = array(i);
			assert(all(eq(
				add(iota(1), a),   // length 3
				mul(c(2), iota(1)) // length unbounded
			)));
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion binop

#pragma region join

	template<iterable I, iterable J>
	class join {
		I i;
		J j;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = int;// std::common_type_t<typename I::value_type, typename J::value_type>;

		join()
		{ }
		join(const I& i, const J& j)
			: i(i), j(j)
		{ }
		join(const join&) = default;
		join& operator=(const join&) = default;
		~join()
		{ }

		auto operator<=>(const join&) const = default;

		explicit operator bool() const
		{
			return i or j;
		}
		join end() const
		{
			return join(i.end(), j.end());
		}
		value_type operator*() const
		{
			if (i) {
				return *i;
			}

			return *j;
		}
		join& operator++()
		{
			if (i) {
				++i;
			}
			else if (j) {
				++j;
			}

			return *this;
		}
	};

#ifdef _DEBUG

	inline bool test_join()
	{
		{
			auto i = take(2, iota<int>(0));
			auto j = take(2, iota<int>(2));
			assert(equal(
				join(i, j),
				take(4, iota<int>(0))
			));
		}
		/*
		{
			join j(unit(1), unit(2));
			assert(j);
			auto j2(j);
			assert(j2);
			assert(j2 == j);
			j = j2;
			assert(!(j != j2));
			assert(equal(j, j2));

			assert(2 == length(j));
			assert(*j == 1);
			++j;
			assert(*j == 2);
			++j;
			assert(!j);
		}
		*/
		return true;
	}

#endif // _DEBUG

#pragma endregion join

#pragma region fold

	// right fold using binop
	template<class Op, iterable I, class T = typename I::value_type>
	class fold : public I {
		Op op;
		T t;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = typename I::difference_type;
		using value_type = T;

		fold()
		{ }
		fold(const Op& op, const I& i, const T& t0)
			: I(i), op(op), t(t0)
		{ }
		fold(const fold&) = default;
		fold& operator=(const fold&) = default;
		~fold()
		{ }

		bool operator==(const fold& f) const
		{
			return t == f.t and I::operator==(f);
		}

		explicit operator bool() const
		{
			return !!I(*this);
		}
		value_type operator*() const
		{
			return op(t, I::operator*());
		}
		fold& operator++()
		{
			t = operator*();
			I::operator++();

			return *this;
		}
	};
	template<iterable I>
	inline auto sum(const I& i)
	{
		return fold(std::plus{}, i, 0);
	}
	template<iterable I, class T = typename I::value_type>
	inline T add(I i)
	{
		if (i) {
			T t = *i;
			return t + add(++i);
		}

		return 0;
	}
	template<iterable I>
	inline auto product(const I& i)
	{
		return fold(std::multiplies{}, i, 1);
	}
	template<iterable I, class T = typename I::value_type>
	inline T mul(I i)
	{
		if (i) {
			T t = *i;
			return t + mul(++i);
		}

		return 1;
	}

#ifdef _DEBUG

	inline bool test_fold()
	{
		{
			fold f(std::plus<int>{}, iota<int>{}, 2);
			auto f2(f);
			f = f2;
			assert(f == f2);
			assert(!(f2 != f));
			assert(f);
			assert(*f == 2 + 0);
			++f;
			assert(*f == 2 + 0 + 1);
			++f;
			assert(*f == 2 + 0 + 1 + 2);
		}
		{
			auto s = sum(iota<int>(1));
			assert(1 == *s);
			s = skip(2, s);
			assert(1 + 2 + 3 == *s);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion fold

}
#define ITERABLE_ARITHMETIC(a,b,c)        \
	template<fms::iterable I, fms::iterable J>      \
	inline auto operator a(const I& i, const J& j) \
	{ return fms::binop(c{}, i, j); }

FMS_ARITHMETIC_OPS(ITERABLE_ARITHMETIC)
#undef ITERABLE_ARITHMETIC

// peephole optimizations
template<fms::iterable I, typename T = typename I::value_type>
inline auto operator+(const I& i, const T& t)
{
	if (t == 0) {
		return i;
	}

	return i + fms::constant(t);
}
template<fms::iterable I, typename T = typename I::value_type>
inline auto operator+(const T& t, const I& i)
{
	if (t == 0) {
		return i;
	}

	return fms::constant(t) + i;
}

template<fms::iterable I, typename T = typename I::value_type>
inline auto operator-(const I& i, const T& t)
{
	if (t == 0) {
		return i;
	}

	return i - fms::constant(t);
}
template<fms::iterable I, typename T = typename I::value_type>
inline auto operator-(const T& t, const I& i)
{
	return fms::constant(t) - i;
}

template<fms::iterable I, typename T = typename I::value_type>
inline auto operator*(const I& i, const T& t)
{
	if (t == 1) {
		return i;
	}

	return i * fms::constant(t);
}
template<fms::iterable I, typename T = typename I::value_type>
inline auto operator*(const T& t, const I& i)
{
	if (t == 1) {
		return i;
	}

	return fms::constant(t) * i;
}

template<fms::iterable I, typename T = typename I::value_type>
inline auto operator/(const I& i, const T& t)
{
	if (t == 1) {
		return i;
	}

	return i / fms::constant(t);
}
template<fms::iterable I, typename T = typename I::value_type>
inline auto operator/(const T& t, const I& i)
{
	return fms::constant(t) / i;
}

template<fms::iterable I, typename T = typename I::value_type>
inline auto operator%(const I& i, const T& t)
{
	return i % fms::constant(t);
}
template<fms::iterable I, typename T = typename I::value_type>
inline auto operator%(const T& t, const I& i)
{
	return fms::constant(t) % i;
}

#define ITERABLE_COMPARISON(a,b,c)                              \
	template<fms::iterable I, fms::iterable J>                  \
	inline auto operator a(const I& i, const J& j)              \
	{ return fms::binop(c{}, i, j); }                           \
                                                                \
	template<fms::iterable I, class T = typename I::value_type> \
	inline auto operator a(const I& i, const T& t)              \
	{ return fms::binop(c{}, i, fms::constant(t)); }            \
                                                                \
	template<fms::iterable I, class T = typename I::value_type> \
	inline auto operator a(const T& t, const I& i)              \
	{ return fms::binop(c{}, fms::constant(t), i); }

FMS_COMPARISON_OPS(ITERABLE_COMPARISON)

template<fms::iterable I>
inline auto operator~(const I& i)
{
	return i == 0;
}

// filter comprehension
template<class P, fms::iterable I>
inline auto operator|(const I& i, const P& p)
{
	return fms::when(p, i);
}
template<fms::iterable I, fms::iterable M>
inline auto operator&(const I& i, const M& m)
{
	return fms::mask(i, m);
}

template<fms::iterable I, fms::iterable J>
inline auto operator,(const I& i, const J& j)
{
	return fms::join(i, j);
}
#endif // 0
