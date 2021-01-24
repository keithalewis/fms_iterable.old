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
	concept iterable = // std::input_or_output_iterator &&
		requires (I i) {
		//std::is_base_of_v<std::forward_iterator_tag, typename I::interator_category>;
		//typename I::iterator_concept;
		typename I::iterator_category;
		typename I::value_type;
		{ !!i /*i.operator bool()*/ } -> std::same_as<bool>;
		{ *i } -> std::convertible_to<typename I::value_type>;
		{ ++i } -> std::same_as<I&>;
		//{ i.operator==(i) const } -> std::same_as<bool>;
		//{ i.begin() } -> std::same_as<I>;
		//{ i.end() } -> std::same_as<I>;
	};

	// "All happy iterables begin alike..."
	template<iterable I>
	inline I begin(const I& i)
	{
		return i;
	}
	// "...but each iterable ends after its own fashion."
	template<iterable I>
	inline auto end(const I& i)
	{
		return i.end();
	}

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
		constant(const T& t)
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
		auto end() const
		{
			return constant(std::numeric_limits<T>::max());
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

		return true;
	}

#endif // _DEBUG

#pragma endregion constant

#pragma region iota

	//??? replace by fold(incr, constant(0))
	template<typename T>
	class iota {
		T t;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		iota(T t = 0)
			: t(t)
		{ }
		iota(const iota&) = default;
		iota& operator=(const iota&) = default;
		~iota()
		{ }

		bool operator==(const iota& i) const
		{
			return t == i.t;
		}
		bool operator!=(const iota& i) const
		{
			return !operator==(i);
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
			++t;

			return *this;
		}
	};
	
#pragma endregion iota

#pragma region until

	// stop when end or p(i) is true
	template<class P, iterable I>
	class until : public I {
		P p;
	public:
		until()
		{ }
		until(P p, const I& i)
			: p(p), I(i)
		{ }
		until(const until&) = default;
		until& operator=(const until&) = default;
		~until()
		{ }

		explicit operator bool() const
		{
			return I::operator!=(I::end()) and !p(*this);
		}
		auto end() const
		{
			return until(p, I::end());
		}
	};

#ifdef _DEBUG

	inline bool test_until()
	{
		{
			iota<int> i;
			auto u = until([](auto i) { return *i == 3; }, i);
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

		return true;
	}

#endif // _DEBUG

#pragma endregion until

#pragma region sentinal

	template<iterable I>
	inline auto sentinal(const I& i, const I& e)
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

#pragma endregion sentinal

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

#pragma region take

	// take first n items
	template<iterable I>
	class take : public I {
		size_t n;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename I::value_type;

		take()
			: n(0)
		{ }
		take(size_t n, const I& i)
			: I(i), n(n)
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

		bool operator==(const take& i) const
		{
			return n == i.n and I::operator==(i);
		}
		auto end()
		{
			return take(0, skip(n, *this));
		}

		explicit operator bool() const
		{
			return n != 0;
		}
		take& operator++()
		{
			if (*this) {
				--n;
				I::operator++();
			}

			return *this;
		}
	};

	// random_access<iterator> : add operator+=

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

		return true;
	}

#endif // _DEBUG

#pragma endregion take

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

#pragma region concatenate

	template<iterable I, iterable J>
	class concatenate {
		I i;
		J j;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::common_type_t<typename I::value_type, typename J::value_type>;

		concatenate()
		{ }
		concatenate(const I& i, const J& j)
			: i(i), j(j)
		{ }
		concatenate(const concatenate&) = default;
		concatenate& operator=(const concatenate&) = default;
		~concatenate()
		{ }

		bool operator==(const concatenate& s) const
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
		concatenate& operator++()
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

	inline bool test_concatenate()
	{
		{
			auto i = take(2, iota<int>(0));
			auto j = take(2, iota<int>(2));
			assert(all(eq(
				concatenate(i, j),
				take(4, iota<int>(0))
			)));
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion concatenate

#pragma region fold

	// right fold using binop
	template<class Op, iterable I>
	class fold {
		using T = typename I::value_type;
		Op op;
		I i;
		T t;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		fold()
		{ }
		fold(const Op& op, const I& i, const T& t0)
			: op(op), i(i), t(t0)
		{ }
		fold(const fold&) = default;
		fold& operator=(const fold&) = default;
		~fold()
		{ }

		bool operator==(const fold& f) const
		{
			return t == f.t and i == f.i;
		}

		explicit operator bool() const
		{
			return !!i;
		}
		value_type operator*() const
		{
			return op(t, *i);
		}
		fold& operator++()
		{
			t = operator*();
			++i;

			return *this;
		}
	};
	template<iterable I>
	inline auto sum(const I& i)
	{
		return fold(std::plus{}, i, 0);
	}
	template<iterable I>
	inline auto prod(const I& i)
	{
		return fold(std::multiplies{}, i, 1);
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

template<fms::iterable I, fms::iterable J>
inline auto operator+(const I& i, const J& j)
{
	return add(i, j);
}

template<fms::iterable I, fms::iterable J>
inline auto operator,(const I& i, const J& j)
{
	return fms::concatenate(i, j);
}