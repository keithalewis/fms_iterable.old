// fms_iterable.h - iterator with operator bool() const to detect the end
#pragma once
#include <execution>
#include <functional>
#include <initializer_list>
#include <list>
#include <numeric>
#include <type_traits>
#include <vector>

namespace fms::iterable {


	template <class I>
	concept input = requires(I i) {
		//typename I::iterator_concept;
		typename I::iterator_category;
		typename I::value_type;
		{ i.operator bool() } -> std::same_as<bool>;
		{ i.operator *() } -> std::convertible_to<typename I::value_type>;
		{ i.operator++() } -> std::same_as<I&>;
		//		{ ++i } -> IsReferenceToBase;
	};

	template <class I>
	concept has_begin = requires(I i) {
		{ i.begin() } -> std::same_as<I>;
	};
	template <class I>
	concept has_end = requires(I i) {
		{ i.end() } -> std::same_as<I>;
	};
	template <class I>
	concept has_back = requires(I i) {
		{ i.back() } -> std::same_as<I>;
	};

	//
	// Stand alone functions
	//

	// All elements equal.
	template <input I, input J>
	inline bool equal(I i, J j) noexcept
	{
		while (i && j) {
			if (*i != *j) {
				return false;
			}
			++i;
			++j;
		}

		return !i && !j; // both done
	}

	// length(i, length(j)) = length(i) + length(j)
	template <input I>
	inline std::size_t length(I i, std::size_t n = 0) noexcept
	{
		while (i) {
			++i;
			++n;
		}

		return n;
	}

	// Drop at most n from the beginning.
	template <input I>
	inline I drop(I i, std::size_t n) noexcept
	{
		while (i && n) {
			++i;
			--n;
		}

		return i;
	}

	// Last element of iterable.
	template <input I>
	inline I back(I i)
	{
		if constexpr (has_back<I>) {
			return i.back();
		}

		I _i(i);

		while (++_i) {
			i = _i;
		}

		return i;
	}

	// For use with STL
	template <class I>
	inline I begin(I i)
	{
		if constexpr (has_begin<I>) {
			return i.begin();
		}

		return i;
	}
	// ++back(i)
	template <input I>
	inline I end(I i)
	{
		if constexpr (has_end<I>) {
			return i.end();
		}

		while (i) {
			++i;
		}

		return i;
	}

	/*
	struct add_postfix_increment {
		template <typename Self>
		auto operator++(this Self && self, int) {
			auto tmp = self;
			++self;
			return tmp;
		}
	};
	*/

	// Make STL container iterable. Assumes lifetime of container.
	template <std::input_iterator I, class T = typename I::value_type>
	class interval  {
		I b, e;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		interval(I b, I e)
			: b(b), e(e)
		{ }

		auto begin() const
		{
			return b;
		}
		auto end() const
		{
			return e;
		}

		// strong equality
		bool operator==(const interval& i) const
		{
			return b == i.b && e == i.e;
		}

		// TODO: size() ???

		explicit operator bool() const
		{
			return b != e;
		}
		value_type operator*() const
		{
			return *b;
		}
		interval& operator++()
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
		/*
		interval operator++(int)
		{
			auto i = *this;

			operator++();

			return i;
		}
		*/
	};
	template<class C> // container
	inline auto make_interval(C& c)
	{
		return interval(c.begin(), c.end());
	}

	// Value class.
	template <class T>
	class list {
		std::list<T> l;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		template <input I>
		list(I i)
		{
			while (i) {
				l.push_back(*i);
				++i;
			}
		}
		// E.g., list({1,2,3})
		list(const std::initializer_list<T>& l)
			: l(l)
		{ }

		// same list
		bool operator==(const list& _l) const { return &l == &_l.l; }

		explicit operator bool() const //override
		{
			return !l.empty();
		}
		value_type operator*() const //override
		{
			return l.front();
		}
		list& operator++() //override
		{
			if (operator bool()) {
				l.pop_front();
			}

			return *this;
		}

		// std::list member function forwarding

		list& push_back(const T& t)
		{
			l.push_back(t);

			return *this;
		}
		list& push_back(T&& t)
		{
			l.push_back(t);

			return *this;
		}
		template <class... Args>
		list& emplace_back(Args&&... args)
		{
			l.emplace_back(args...);

			return *this;
		}
	};

	// Value class.
	template <class T>
	class vector {
		std::vector<T> v;
		std::vector<T>::const_iterator vi;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		vector()
		{  }
		vector(std::size_t n, const T* pt)
			: v(pt, pt + n), vi(v.begin())
		{ }
		// E.g., vector({1,2,3})
		vector(const std::initializer_list<T>& i)
			: v(i), vi(v.begin())
		{ }
		vector(const vector& v_)
			: v(v_.v), vi(v.begin())
		{ }
		vector(vector&& v_) noexcept
			: v(std::move(v_.v)), vi(v.begin())
		{ }
		vector& operator=(const vector& v_)
		{
			if (this != &v_) {
				v = v_.v;
				vi = v.begin();
			}

			return *this;
		}
		vector& operator=(vector&& v_)
		{
			if (this != &v_) {
				v = std::move(v_.v);
				vi = v.begin();
			}

			return *this;
		}
		~vector()
		{ }

		auto begin() const
		{
			return v.begin();
		}
		auto end() const
		{
			return v.end();
		}

		// Strong equality.
		bool operator==(const vector& _v) const 
		{
			return vi == _v.vi && &v == &_v.v; 
		}

		explicit operator bool() const
		{
			return vi != v.end();
		}
		T operator*() const
		{
			return *vi;
		}
		vector& operator++()
		{
			if (operator bool()) {
				++vi;
			}

			return *this;
		}

		// Multi-pass
		vector& reset()
		{
			vi = v.begin();

			return *this;
		}

		// std::vector member function forwarding

		vector& push_back(const T& t)
		{
			v.push_back(t);

			return *this;
		}
		vector& push_back(T&& t)
		{
			v.push_back(t);

			return *this;
		}
		template <class... Args>
		vector& emplace_back(Args&&... args)
		{
			v.emplace_back(args...);

			return *this;
		}
	};
	// Cache iterable values.
	template <input I, class T = typename I::value_type>
	inline auto make_vector(I i)
	{
		vector<T> v;

		while (i) {
			v.push_back(*i);
			++i;
		}
		v.reset();

		return v;
	}


	// Constant iterable: {c, c, c, ...}
	template <class T>
	class constant {
		T c;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		constant(T c) noexcept
			: c(c)
		{ }

		bool operator==(const constant& _c) const //= default;
		{
			return c == _c.c;
		}

		explicit operator bool() const noexcept
		{
			return true;
		}
		value_type operator*() const noexcept
		{
			return c;
		}
		constant& operator++() noexcept
		{
			return *this;
		}
	};

	// t, t + 1, t + 2, ...
	template <class T>
	class iota {
		T t;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		iota(T t = 0) noexcept
			: t(t)
		{ }

		bool operator==(const iota& i) const { return t == i.t; }

		explicit operator bool() const noexcept { return true; }
		value_type operator*() const noexcept { return t; }
		iota& operator++() noexcept
		{
			++t;

			return *this;
		}
	};

	// tn, tn*t, tn*t*t, ...
	template <class T>
	class power {
		T t, tn;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		power(T t, T tn = 1)
			: t(t), tn(tn)
		{ }

		bool operator==(const power& p) const { return t == p.t && tn == p.tn; }

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return tn;
		}
		power& operator++()
		{
			tn *= t;

			return *this;
		}
	};

	// 1, 1, 2, 6, 24, ...
	template <class T = double>
	class factorial {
		T t, n;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		factorial(T t = 1)
			: t(t), n(1)
		{ }

		bool operator==(const factorial& f) const //= default;
		{
			return t == f.t && n == f.n;
		}

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return t;
		}
		factorial& operator++()
		{
			t *= n++;

			return *this;
		}
	};

	// 1, n, n*(n-1)/2, ..., 1
	template <class T = std::size_t>
	class choose {
		T n, k, nk;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		choose(T n)
			: n(n), k(0), nk(1)
		{ }

		bool operator==(const choose& c) const = default;

		explicit operator bool() const
		{
			return k <= n;
		}
		value_type operator*() const
		{
			return nk;
		}
		choose& operator++()
		{
			if (k <= n) {
				nk *= n - k;
				++k;
				nk /= k;
			}

			return *this;
		}
	};

	// Unsafe pointer interface.
	template <class T>
	class pointer {
		T* p;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		// pointer() is empty iterator
		pointer(T* p = nullptr) noexcept
			: p(p)
		{ }

		bool operator==(const pointer& _p) const //= default;
		{
			return p == _p.p;
		}

		explicit operator bool() const noexcept
		{
			return p != nullptr; // unsafe
		}
		value_type operator*() const noexcept
		{
			return *p;
		}
		pointer& operator++() noexcept
		{
			++p;

			return *this;
		}
	};

	// Terminate on 0 value.
	template <class T>
	class null_terminated_pointer {
		T* p;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		null_terminated_pointer(T* p) noexcept
			: p(p)
		{ }

		bool operator==(const null_terminated_pointer& _p) const //= default;
		{
			return p == _p.p;
		}

		explicit operator bool() const
		{
			return *p != 0;
		}
		value_type operator*() const
		{
			return *p;
		}
		null_terminated_pointer& operator++()
		{
			if (operator bool())
				++p;

			return *this;
		}
	};

	// Iterable having exactly one element. {t}
	template <class T>
	class once {
		T t;
		bool b;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		once(T t) noexcept
			: t(t), b(true)
		{ }

		bool operator==(const once& o) const //= default;
		{
			return t == o.t && b == o.b;
		}

		explicit operator bool() const noexcept
		{
			return b;
		}
		value_type operator*() const noexcept
		{
			return t;
		}
		once& operator++() noexcept
		{
			b = false;

			return *this;
		}
	};

	// Take at most n elements.
	template <input I, class T = I::value_type>
	class take {
		I i;
		std::size_t n;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		take(const I& i, std::size_t n)
			: i(i), n(n)
		{ }

		bool operator==(const take& t) const //= default;
		{
			return i == t.i && n == t.n;
		}

		explicit operator bool() const noexcept
		{
			return i && n > 0;
		}
		value_type operator*() const noexcept
		{
			return *i;
		}
		take& operator++() noexcept
		{
			if (n) {
				++i;
				--n;
			}

			return *this;
		}
	};

	// Assumes lifetime of a[N].
	template <class T, std::size_t N>
	inline auto array(T(&a)[N]) noexcept
	{
		return take(pointer<T>(a), N);
	}

	// i0 then i1
	template <input I0, input I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class concatenate {
		I0 i0;
		I1 i1;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		concatenate(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{ }

		bool operator==(const concatenate& i) const //= default;
		{
			return i0 == i.i0 && i1 == i.i1;
		}

		explicit operator bool() const
		{
			return i0 || i1;
		}
		value_type operator*() const
		{
			return i0 ? *i0 : *i1;
		}
		concatenate& operator++()
		{
			if (i0) {
				++i0;
			}
			else {
				++i1;
			}

			return *this;
		}
	};

	// Sorted i0 and i1 in order. Equivalent (!< and !>) elements are repeated.
	template <input I0, input I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class merge {
		I0 i0;
		I1 i1;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		merge(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{ }

		bool operator==(const merge& i) const //= default;
		{
			return i0 == i.i0 && i1 == i.i1;
		}

		explicit operator bool() const
		{
			return i0 || i1;
		}
		value_type operator*() const
		{
			if (i0 && i1) {
				if (*i0 < *i1) {
					return *i0;
				}
				else if (*i1 < *i0) {
					return *i1;
				}
				else {
					return *i0;
				}
			}

			return i0 ? *i0 : *i1;
		}
		merge& operator++()
		{
			using std::swap;

			if (i0 && i1) {
				if (*i0 < *i1) {
					++i0;
				}
				else if (*i1 < *i0) {
					++i1;
				}
				else {
					++i0;
					swap(i0, i1);
				}
			}
			else {
				if (i0) {
					++i0;
				}
				else if (i1) {
					++i1;
				}
			}

			return *this;
		}
	};

	// f(), ...
	template <class F, class T = std::invoke_result_t<F>>
	class call {
		const F& f;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		call(const F& f)
			: f(f)
		{ }

		bool operator==(const call& c) const 
		{
			return f == c.f; 
		}

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return f();
		}
		call& operator++()
		{
			return *this;
		}
	};

	// Apply a function to elements of an iterable.
	// f(*i), f(*++i), f(*++i), ...
	template <class F, input I, class T = typename I::value_type, class U = std::invoke_result_t<F, T>>
	class apply {
		const F& f;
		I i;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = U;

		apply(const F& f, const I& i)
			: f(f), i(i)
		{ }
		apply(const apply& a)
			: f(a.f), i(a.i)
		{ }
		apply& operator=(const apply& a)
		{
			if (this != &a) {
				// f = a.f;
				i = a.i;
			}

			return *this;
		}
		~apply() 
		{ }

		bool operator==(const apply& a) const
		{
			return f == a.f && i == a.i;
		}

		explicit operator bool() const
		{
			return i.operator bool();
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

	// TODO: apply(f, *i0, *i1, ...), apply(f, {*++i0, *++i1, ...}), ...

	// Apply a binary operation to elements of two iterable.
	template <class BinOp, input I0, input I1, class T0 = typename I0::value_type, class T1 = typename I1::value_type, class T = std::invoke_result_t<BinOp, T0, T1>>
	class binop {
		const BinOp& op;
		I0 i0;
		I1 i1;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		binop(const BinOp& op, I0 i0, I1 i1)
			: op(op), i0(i0), i1(i1)
		{ }
		binop(const binop& o)
			: op(o.op), i0(o.i0), i1(o.i1)
		{ }
		binop& operator=(const binop& o)
		{
			if (this != &o) {
				i0 = o.i0;
				i1 = o.i1;
			}

			return *this;
		}
		~binop() { }

		bool operator==(const binop& o) const
		{
			return op == o.op && i0 == o.i0 && i1 == o.i1;
		}

		explicit operator bool() const
		{
			return i0 && i1;
		}
		value_type operator*() const
		{
			return op(*i0, *i1);
		}
		binop& operator++()
		{
			++i0;
			++i1;

			return *this;
		}
	};

	// Elements satisfying predicate.
	template <class P, input I, class T = typename I::value_type>
	class filter {
		const P& p;
		I i;

		void incr()
		{
			while (i && ++i && !p(*i)) {
				;
			}
		}
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		filter(const P& p, const I& i)
			: p(p), i(i)
		{
			incr();
		}
		filter(const filter& a)
			: p(a.p), i(a.i)
		{ }
		filter& operator=(const filter& a)
		{
			if (this != &a) {
				i = a.i;
			}

			return *this;
		}
		~filter()
		{ }

		bool operator==(const filter& a) const
		{
			return p == a.p and i == a.i;
		}

		explicit operator bool() const
		{
			return i.operator bool();
		}
		value_type operator*() const
		{
			return *i;
		}
		filter& operator++()
		{
			incr();

			return *this;
		}
	};

	// Stop at first element satisfying predicate.
	template <class P, input I, class T = typename I::value_type>
	class until {
		const P& p;
		I i;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		until(const P& p, const I& i)
			: p(p), i(i)
		{ }
		until(const until& a)
			: p(a.p), i(a.i)
		{ }
		until& operator=(const until& a)
		{
			if (this != &a) {
				i = a.i;
			}

			return *this;
		}
		~until()
		{ }

		bool operator==(const until& a) const
		{
			return p == a.p and i == a.i;
		}

		explicit operator bool() const
		{
			return i && !p(*i);
		}
		value_type operator*() const
		{
			return *i;
		}
		until& operator++()
		{
			++i;

			return *this;
		}
	};

	// Right fold: t, op(t, *i), op(op(t, *i), *++i), ...
	template <class BinOp, input I, class T = typename I::value_type>
	class fold {
		const BinOp& op;
		I i;
		T t;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		fold(const BinOp& op, const I& i, T t = 0)
			: op(op), i(i), t(t)
		{ }
		fold(const fold& f)
			: op(f.op), i(f.i), t(f.t)
		{ }
		fold& operator=(const fold& f)
		{
			if (this != &f) {
				i = f.i;
				t = f.t;
			}

			return *this;
		}
		~fold() { }

		bool operator==(const fold& f) const
		{
			return i == f.i && t == f.t; // BinOp is part of type
		}

		explicit operator bool() const
		{
			return i.operator bool();
		}
		value_type operator*() const
		{
			return t;
		}
		fold& operator++()
		{
			if (i) {
				t = op(t, *i);
				++i;
			}

			return *this;
		}
	};
	template <input I, class T = typename I::value_type>
	inline auto sum(I i, T t = 0)
	{
		while (i) {
			t += *i;
			++i;
		}

		return t;
	}
	template <class E, input I, class T = typename I::value_type>
	inline auto sum(I i, T t = 0, E e = std::execution::seq)
	{
		return std::reduce(e, begin(i), end(i), t);
	}

	template <input I, class T = typename I::value_type>
	inline auto prod(I i, T t = 1)
	{
		while (i) {
			t *= *i;
			++i;
		}

		return t;
	}
	// inline auto horner(I i, T x, T t = 1)

	// d(i[1], i[0]), d(i[2], i[1]), ...
	template <input I, class T = typename I::value_type, class D = std::minus<T>, typename U = std::invoke_result_t<D, T, T>>
	class delta {
		const D& d;
		I i;
		T t, _t;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = U;

		delta(const I& _i, const D& _d = std::minus<T>{})
			: d(_d), i(_i), t{}, _t{}
		{
			if (i) {
				t = *i;
				++i;
				_t = i ? *i : t;
			}
		}
		delta(const delta& _d)
			: d(_d.d), i(_d.i), t(_d.t), _t(_d._t)
		{ }
		delta& operator=(const delta& _d)
		{
			if (this != &_d) {
				i = _d.i;
				t = _d.t;
				_t = _d._t;
			}

			return *this;
		}
		~delta()
		{ }

		bool operator==(const delta& _d) const
		{
			return i == _d.i && t == _d.t && _t == _d._t;
		}

		explicit operator bool() const
		{
			return i.operator bool();
		}
		value_type operator*() const
		{
			return d(*i, t);
		}
		delta& operator++()
		{
			if (i) {
				t = *i;
				++i;
			}

			return *this;
		}
	};

	// uptick + downtick = delta
	template <input I, class T = typename I::value_type>
	inline auto uptick(I i)
	{
		return delta(i, [](T a, T b) { return std::max<T>(b - a, 0); });
	}
	template <input I, class T = typename I::value_type>
	inline auto downtick(I i)
	{
		return delta(i, [](T a, T b) { return std::min<T>(b - a, 0); });
	}

} // namespace fms::iterable

#define FMS_ITERABLE_OPERATOR(X) \
    X(+, std::plus<T> {})        \
    X(-, std::minus<T> {})       \
    X(*, std::multiplies<T> {})  \
    X(/, std::divides<T> {})     \
    X(%, std::modulus<T> {})

#define FMS_ITERABLE_OPERATOR_FUNCTION(OP, OP_)              \
    template <fms::iterable::input I,                        \
        fms::iterable::input J,                              \
        class T = std::common_type_t<typename I::value_type, \
            typename J::value_type>>                         \
    inline auto operator OP(const I& i, const J& j)          \
    {                                                        \
        return fms::iterable::binop(OP_, i, j);              \
    }

FMS_ITERABLE_OPERATOR(FMS_ITERABLE_OPERATOR_FUNCTION)
#undef FMS_ITERABLE_OPERATOR_FUNCTION

template <fms::iterable::input I, fms::iterable::input J>
inline auto operator,(const I& i, const J& j)
{
	return fms::iterable::concatenate(i, j);
}

/*
#define FMS_ITERABLE_RELATION(X) \
		X(==, std::equal_to<T>{}) \
		X(!=, std::not_equal_to<T>{}) \
		X(<, std::less<T>{}) \
		X(<=, std::less_equal<T>{}) \
		X(>, std::greater<T>{}) \
		X(>=, std::greater_equal<T>{}) \

#define FMS_ITERABLE_RELATION_INLINE(a, b) \
template<fms::iterable::input I, class T = typename I::value_type> \
inline auto operator a(const I& i, T t) { return fms::iterable::filter([t](T u)
{ return u a t; }, i); } #undef FMS_ITERABLE_RELATION_INLINE
template<fms::iterable::input I, class T = typename I::value_type>
inline auto operator==(const I& i, T t) { return fms::iterable::filter([t](T u)
{ return u == t; }, i); } template<fms::iterable::input I, class T = typename
I::value_type> inline auto operator!=(const I& i, T t) { return
fms::iterable::filter([t](T u) { return u != t; }, i); }
*/
template <fms::iterable::input I, class T = typename I::value_type>
inline auto
operator<(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u < t; }, i);
}
template <fms::iterable::input I, class T = typename I::value_type>
inline auto
operator<=(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u <= t; }, i);
}
template <fms::iterable::input I, class T = typename I::value_type>
inline auto
operator>(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u > t; }, i);
}
template <fms::iterable::input I, class T = typename I::value_type>
inline auto
operator>=(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u >= t; }, i);
}
