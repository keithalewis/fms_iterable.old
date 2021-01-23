// fms_iterable.h - iterators with `explicit operator bool() const`
#pragma once
#include <concepts>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>

// to make tests return false instead of abort
// #define assert(e) if (!(e)) return false

namespace fms {

	template<class I>
	concept iterable = requires (I i) {
		//std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
		typename I::iterator_concept;
		typename I::iterator_category;
		typename I::value_type;
		{ i.operator bool() } -> std::same_as<bool>;
		{ *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		//{ i.operator==(i) const } -> std::same_as<bool>;
		//{ i.begin() } -> std::same_as<I>;
		//{ i.end() } -> std::same_as<I>;
	};

#pragma region all

	// every element is convertible to true
	template<iterable I>
	inline constexpr bool all(I i)
	{
		return i ? (*i and all(++i)) : true;
	}

#ifdef _DEBUG

	template<iterable I>
	inline bool test_all(const I& is)
	{
		bool b = true; // all({}) = true
		for (auto i : is) {
			if (!i) {
				b = false;
			}
		}

		assert(b == all(is));
	}

#endif // _DEBUG

#pragma endregion all

#pragma region any

	// some element is convertible to true
	template<iterable I>
	inline constexpr bool any(I i)
	{
		return i ? (*i or any(++i)) : false;
	}
#ifdef _DEBUG

	template<iterable I>
	inline bool test_any(const I& is)
	{
		bool b = false; // any({}) = false
		for (auto i : is) {
			if (!i) {
				b = true;
			}
		}

		assert(b == any(is));
	}

#endif // _DEBUG

#pragma endregion any

	template<iterable I>
	inline I drop(size_t n, I i)
	{
		while (n and i) { // !!!specialize for random access
			--n;
			++i;
		}

		return i;
	}

	// number of items in sequence
	// length(s, length(t)) = length(s) + length(t)
	template<iterable I>
	inline size_t length(I i, size_t n = 0)
	{
		while (i) {
			++n;
			++i;
		}

		return n;
	}
	template<iterable I>
	inline size_t size(I i, size_t n = 0)
	{
		return length(i, n);
	}


#pragma region array

	template<class T>
	class array {
		size_t n;
		T* a;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		array(size_t n = 0, T* a = nullptr)
			: n(n), a(a)
		{ }
		template<size_t N>
		array(T(&a)[N])
			: n(N), a(a)
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

		bool operator==(const array& _a) const
		{
			return n == _a.n and a == _a.a;
		}
		auto begin()
		{
			return *this;
		}
		auto end()
		{
			return array(0, a + n);
		}
		
		explicit operator bool() const
		{
			return n != 0;
		}
		value_type operator*() const
		{
			return *a;
		}
		/*
		value_type& operator*()
		{
			return *a;
		}
		*/
		array& operator++()
		{
			if (*this) {
				--n;
				++a;
			}

			return *this;
		}
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
		/*
		{
			T a_[] = { 1,2,3 };
			array<T> a(a_);

			auto a0 = *a;
			for (auto& ai : a) {
				assert(ai == a0);
				++a0;
			}
		}
		*/
		{
			T a_[] = { 1,2,3 };
			array<T> a(a_);

			auto a0 = *a;
			for (auto ai : a) {
				assert(ai == a0);
				++a0;
			}
		}
		{
			T a_[] = { 1,2,3 };
			array<T> a(a_);
			assert(6 == std::accumulate(a.begin(), a.end(), 0));
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion array

#pragma region constant

	template<typename T>
	class constant {
		T t;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		constant()
		{ }
		constant(const value_type& t)
			: t(t)
		{ }
		constant(const constant&) = default;
		constant& operator=(const constant&) = default;
		~constant()
		{ }

		bool operator==(const constant& c) const
		{
			return t == c.t;
		}
		auto begin() const
		{
			return *this;
		}
		auto end() const
		{
			return constant(~t);
		}
		
		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return t;
		}
		constant& operator++()
		{
			return *this;
		}
	};
	template<typename T>
	inline auto c(const T& t)
	{
		return constant(t);
	}

#ifdef _DEBUG

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
			assert(c.begin() != c.end());
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion constant

#pragma region iota
	template<typename T>
	class iota {
		T t, dt;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		iota()
			: t(0), dt(1)
		{ }
		iota(const T& t, T dt = 1)
			: t(t), dt(dt)
		{ }
		iota(const iota&) = default;
		iota& operator=(const iota&) = default;
		~iota()
		{ }

		bool operator==(const iota& c) const
		{
			return t == c.t and dt == c.dt;
		}
		auto begin() const
		{
			return *this;
		}
		auto end() const
		{
			return iota(std::numeric_limits<T>::max());
		}

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return t;
		}
		iota& operator++()
		{
			t = t + dt;

			return *this;
		}

	};
#pragma endregion iota

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
		apply begin() const
		{
			return *this;
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
			for (auto ai = a.begin(); ai != a.end(); ++ai) {
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

	// apply binary operator 
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
	// ne, lt, le, ge, gt
	template<iterable I, iterable J>
	inline auto add(const I& i, const J& j)
	{
		return binop(std::plus{}, i, j);
	}
	template<iterable I, iterable J>
	inline auto sub(const I& i, const J& j)
	{
		return binop(std::minus{}, i, j);
	}
	template<iterable I, iterable J>
	inline auto mul(const I& i, const J& j)
	{
		return binop(std::multiplies{}, i, j);
	}
	template<iterable I, iterable J>
	inline auto div(const I& i, const J& j)
	{
		return binop(std::divides{}, i, j);
	}
	template<iterable I, iterable J>
	inline auto mod(const I& i, const J& j)
	{
		return binop(std::modulus{}, i, j);
	}
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

#pragma region take

	// take first n items
	template<iterable I>
	class take {
		size_t n;
		I i;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		take()
			: n(0)
		{ }
		take(size_t n, const I& i)
			: n(n), i(i)
		{ }
		take(const take&) = default;
		take& operator=(const take&) = default;
		~take()
		{ }

		// remaining size
		size_t size() const
		{
			return n;
		}

		bool operator==(const take & _i) const
		{
			return n == _i.n and i == _i.i;
		}
		auto begin()
		{
			return *this;
		}
		auto end()
		{
			return drop(n, *this);
		}

		explicit operator bool() const
		{
			return n != 0;
		}
		value_type operator*() const
		{
			return *i;
		}
		/*
		value_type& operator*()
		{
			return *i;
		}
		*/
		take& operator++()
		{
			if (*this) {
				--n;
				++i;
			}

			return *this;
		}
	};

#ifdef _DEBUG

	template<typename T>
	inline bool test_take()
	{
		{
			auto t = take(2, iota<T>{});
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

		return true;
	}

#endif // _DEBUG

#pragma endregion take
}

template<fms::iterable I, fms::iterable J>
inline auto operator+(const I& i, const J& j)
{
	return add(i, j);
}
