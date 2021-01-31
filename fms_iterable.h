// fms_iterable.h - iterators with `explicit operator bool() const`
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif
#include <cmath>
#include <concepts>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <numeric>
#include <utility>

// to make tests return false instead of abort
// #define assert(e) if (!(e)) { return false; }

#define FMS_ARITHMETIC_OPS(X)     \
	X(+, add, std::plus)          \
	X(-, sub, std::minus)         \
	X(*, mul, std::multiplies)    \
	X(/, div, std::divides)       \
	X(%, mod, std::modulus)       \

#define FMS_COMPARISON_OPS(X)     \
	X(==, eq, std::equal_to)      \
	X(!=, ne, std::not_equal_to)  \
	X(> , gt, std::greater)       \
	X(< , lt, std::less)          \
	X(>=, ge, std::greater_equal) \
	X(<=, le, std::less_equal)    \

#define FMS_LOGICAL_OPS(X)        \
	X(&&, and, std::logical_and)  \
	X(||, or ,std::logical_or)    \

#define FMS_BITWISE_OPS(X)        \
	X(&, AND, std::bit_and)       \
	X(|, OR , std::bit_or)        \
	X(^, XOR, std::bit_xor)       \

namespace {
	// relations
	template<typename I, class R>
	struct relation {
		using T = typename I::value_type;
		T t;
		relation(const T& t)
			: t(t)
		{ }
		bool operator()(const I& i) const
		{
			return R{}(*i, t);
		}
	};
}

/*
//!!! move to namespace
#define UNTIL_COMPARISON(a,b,c) \
	template<typename I, class T = typename I::value_type> \
	using b = relation<I, c<T>>; \
	
	FMS_COMPARISON_OPS(UNTIL_COMPARISON)
#undef UNTIL_COMPARISON
*/

namespace fms {

	template<class I>
	concept iterable = 
		requires (I i) {
		//std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
		//typename I::iterator_concept;
		typename I::iterator_category;
		typename I::value_type;
		{ !!i } -> std::same_as<bool>;
		{  *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		//{ i.operator==(i) const } -> std::same_as<bool>;
		//{ i.end() } -> std::same_as<I>;
	};

	// "All happy iterables begin alike..."
	template<iterable I>
	inline I begin(const I& i)
	{
		return i;
	}
	// "...but each iterable ends in its own fashion."
	template<iterable I>
	inline auto end(const I& i)
	{
		return i.end();
	}

	// all values are equal
	template<iterable I, iterable J>
	inline bool equal(I i, J j)
	{
		while (i and j) {
			if (*i != *j) {
				return false;
			}
			++i;
			++j;
		}

		return !i and !j;
	}

	/*
	// default iterable policies
	template<iterable I>
	inline auto op_bool = [](const I& i) { return !!i; };
	template<iterable I>
	inline auto op_true = [](const I&) { return true; };

	template<iterable I>
	inline auto op_star = [](const I& i) { return *i; };
	template<iterable I, typename T>
	inline auto op_static_star = [](const T& t) {
		return [t](const I&) { static T t; return t; };
	};

	template<iterable I>
	inline auto op_incr = [](I& i) { ++i; };
	template<iterable I>
	inline auto op_noincr = [](I&) { ; };

	// dependency inject operator bool, star(*), increment(++), and types.
	template<class I, class OB, class OS, class OI,
		class CAT = std::forward_iterator_tag,
		class TYPE = std::invoke_result_t<decltype(&I::operator*)()>>
	class policy : public I {
		std::optional<OB> ob;
		std::optional<OS> os;
		std::optional<OI> oi;
		// bypass operator=
		void assign(const OB& _ob)
		{
			ob.reset();
			if (_ob) {
				ob.emplace(_ob);
			}
		}
		//...
	public:
		using iterator_concept = typename CAT;
		using iterator_category = typename CAT;
		using value_type = typename TYPE;

		policy(const I& i, const OB& ob = op_bool, const OS& os = op_star, const OI& oi = op_incr)
			: I(i), ob(ob), os(os), oi(oi)
		{ }
		policy(const policy& p) = default;
		policy& operator=(const policy& p)
		{
			if (this != &p) {
				ob.reset();
				if (p.ob) {
					ob.emplace(p.ob);
				}
			}

			return *this;
		}


		explicit operator bool() const
		{
			return ob(*this);
		}
		value_type operator*() const
		{
			return os(*this);
		}
		policy& operator++()
		{
			oi(*this);

			return *this;
		}
	};
	*/

}

#include "fms_function.h"
#include "fms_numeric.h"

namespace fms {

#pragma region pointer

	// create an iterable from a random access pointer
	// `explicit operator bool() const` is always dangerously true
	template<class T>
	class pointer {
		T* p;
	public:
		using iterator_concept = typename std::iterator_traits<T*>::iterator_concept;
		using iterator_category = typename std::iterator_traits<T*>::iterator_category;
		using difference_type = typename std::iterator_traits<T*>::difference_type;
		using reference = typename T&;
		using value_type = T;

		pointer(T* p = nullptr)
			: p(p)
		{ }
		pointer(const pointer&) = default;
		pointer& operator=(const pointer&) = default;
		~pointer()
		{ }

		auto operator<=>(const pointer&) const = default;

		explicit operator bool() const
		{
			return true; // does not check if valid
		}
		auto end() const
		{
			return pointer(nullptr);
		}

		value_type operator*() const
		{
			return *p;
		}
		reference operator*()
		{
			return *p;
		}

		value_type operator[](difference_type n) const
		{
			return p[n];
		}
		reference operator[](difference_type n)
		{
			return p[n];
		}
		
		pointer& operator++()
		{
			++p;

			return *this;
		}
		pointer operator++(int)
		{
			auto p_ = *this;

			++p;

			return p_;
		}
		pointer& operator--()
		{
			--p;

			return *this;
		}
		pointer operator--(int)
		{
			auto p_ = *this;

			--p;

			return p_;
		}

		pointer& operator+=(difference_type n)
		{
			p += n;

			return *this;
		}
		pointer& operator-=(difference_type n)
		{
			p -= n;

			return *this;
		}
	};

} // namespace fms

template<typename T>
inline auto operator+(fms::pointer<T> p, typename fms::pointer<T>::difference_type n)
{
	return p += n;
}
template<typename T>
inline auto operator+(typename fms::pointer<T>::difference_type n, fms::pointer<T> p)
{
	return p += n;
}
template<typename T>
inline auto operator-(fms::pointer<T> p, typename fms::pointer<T>::difference_type n)
{
	return p -= n;
}

namespace fms {

	#ifdef _DEBUG

	inline bool test_pointer()
	{
		{
			int i[] = { 1,2,3 };
			pointer<int> p(i);
			assert(p);
			auto p2(p);
			assert(p2);
			assert(p2 == p);
			p = p2;
			assert(p);
			assert(!(p != p2));

			assert(*p == i[0]);
			
			++p;
			assert(p);
			assert(*p == i[1]);

			p += 1;
			assert(*p == i[2]);
			p -= 1;
			p += 1;

			p = p - 1;
			assert(*p == i[1]);
			
			p = p + 1;
			assert(*p == i[2]);

			assert(*p-- == i[2]);
			assert(*p == i[1]);

			assert(*p++ == i[1]);
			assert(*p == i[2]);
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion pointer
#pragma region array

	template<class T>
	class array : public pointer<T> {
		size_t n;
	public:
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
		auto end() const
		{
			return array(0, *this + n);
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
			array<T> a(a_);
			assert(6 == std::accumulate(begin(a), end(a), 0));
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

#pragma region null
	
	// use zero value ends iterable
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
			static_assert(std::is_same_v<decltype(n)::value_type, const char>);
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

#pragma region take

	// take first (>0) or last (<0) n items
	// same as done(countdown(n), i)
	template<iterable I>
	class take : public I {
		size_t n;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		//using value_type = typename I::value_type;

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
		auto end() const
		{
			return take(0, skip(n, *this)); //!!! get rid of skip
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
		//??? constexp operators/enable_if/requires
	};
	/*
	// e.g., take_(3)(iota<int>(0))
	template<iterable I>
	inline auto take_(long n)
	{
		return [n](const I& i) { return take<I>(n, i); };
	}
	*/

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

		return true;
	}

#endif // _DEBUG

#pragma endregion take

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
		auto end() const
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
		auto end() const
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
		auto end() const
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
		auto end() const
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
		auto end() const
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
		using value_type = typename U;

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
		auto end() const
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
			apply a(std::negate{}, i);
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
			apply a(std::negate{}, iota<int>{});
			apply b([](int i) { return -i; }, iota<int>{});
			for (int i : {1, 2, 3}) {
				assert(*++a == -i); 
				assert(*++b == -i);
			}
		}
		{
			apply a(std::negate{}, iota<int>{});
			int i = 0;
			for (auto ai = begin(a); ai != end(a); ++ai) {
				assert(*ai == -i);
				++i;
				if (i == 3)
					break;
			}
		}
		{
			apply a(std::negate{}, iota<int>{});
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
		using value_type = std::common_type_t<typename I::value_type, typename J::value_type>;

		join()
		{ }
		join(const I& i, const J& j)
			: i(i), j(j)
		{ }
		join(const join&) = default;
		join& operator=(const join&) = default;
		~join()
		{ }

		bool operator==(const join& s) const
		{
			i == s.i and j == s.j;
		}
		explicit operator bool() const
		{
			return i or j;
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
			assert(all(eq(
				join(i, j),
				take(4, iota<int>(0))
			)));
		}

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
			fold f(std::plus{}, iota<int>{}, 2);
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

template<fms::iterable I, fms::iterable J>
inline auto operator,(const I& i, const J& j)
{
	return fms::join(i, j);
}

// comprehension
template<class P, fms::iterable I>
inline auto operator|(const I& i, const P& p)
{
	return fms::when(p, i);
}
template<fms::iterable I, fms::iterable M>
inline auto operator|(const I& i, const M& m)
{
	return fms::mask(i, m);
}