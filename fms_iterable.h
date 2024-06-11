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

	template <class I, class T = typename I::value_type>
	concept input = requires(I i) {
		//typename I::iterator_concept;
		typename I::iterator_category;
		typename I::value_type;
		{ i.operator bool() } -> std::same_as<bool>;
		{ i.operator *() } -> std::convertible_to<T>;
		{ i.operator++() } -> std::same_as<I&>;
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
	constexpr bool equal(I i, J j) noexcept
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
	constexpr std::size_t length(I i, std::size_t n = 0) noexcept
	{
		if constexpr (has_end<I>) {
			return n + std::distance(i, i.end());
		}

		while (i) {
			++i;
			++n;
		}

		return n;
	}

	// Drop at most n from the beginning.
	template <input I>
	constexpr I drop(I i, std::size_t n) noexcept
	{
		using category = typename I::iterator_category;

		if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category>) {
			std::advance(i, std::min(n, length(i)));
		}
		else {
			while (i && n) {
				++i;
				--n;
			}
		}

		return i;
	}

	// Last element of iterable.
	template <input I>
	constexpr I back(I i)
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
	constexpr I begin(I i)
	{
		if constexpr (has_begin<I>) {
			return i.begin();
		}

		return i;
	}
	// ++back(i)
	template <input I>
	constexpr I end(I i)
	{
		if constexpr (has_end<I>) {
			return i.end();
		}

		while (i) {
			++i;
		}

		return i;
	}

	/* TODO: use when deducing this supported
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
	class interval {
		I b, e;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		constexpr interval(I b, I e)
			: b(b), e(e)
		{ }

		constexpr auto begin() const
		{
			return b;
		}
		constexpr auto end() const
		{
			return e;
		}

		// same container
		constexpr bool operator==(const interval& i) const
		{
			return b == i.b && e == i.e;
		}

		// TODO: size() ???

		constexpr explicit operator bool() const
		{
			return b != e;
		}
		constexpr value_type operator*() const
		{
			return *b;
		}
		constexpr interval& operator++()
		{
			if (operator bool()) {
				++b;
			}

			return *this;
		}
		constexpr interval operator++(int)
		{
			auto i{*this};

			operator++();

			return i;
		}
	};
	template<class C> // container
	constexpr auto make_interval(C& c)
	{
		return interval(c.begin(), c.end());
	}

	// TODO: remove???
	// Value class.
	template <class T>
	class list {
		std::list<T> l;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		template <input I>
			requires std::same_as<T, typename I::value_type>
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

		bool operator==(const list& _l) const = default;

		auto begin() const
		{
			return l.begin();
		}
		auto end() const
		{
			return l.end();
		}

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
	template <input I, class T = typename I::value_type>
	inline auto make_list(I i)
	{
		return list<T>(i);
	}

	// TODO: remove???
	// Value type.
	template <class T>
	class vector {
		std::vector<T> v;
		size_t i;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		vector()
			: v{}, i(0)
		{  }
		template<input I>
			requires std::same_as<T, typename I::value_type>
		vector(I i)
			: v{}, i(0)
		{
			while (i) {
				v.push_back(*i);
				++i;
			}
		}
		vector(std::size_t n, const T* pt)
			: v(pt, pt + n), i(0)
		{ }
		// E.g., vector({1,2,3})
		vector(const std::initializer_list<T>& i)
			: v(i), i(0)
		{ }
		vector(const vector& v_)
			: v(v_.v), i(v_.i)
		{ }
		vector(vector&& v_) noexcept
			: v(std::move(v_.v)), i(v_.i)
		{ }
		vector& operator=(const vector& v_)
		{
			if (this != &v_) {
				v = v_.v;
				i = v_.i;
			}

			return *this;
		}
		vector& operator=(vector&& v_) noexcept
		{
			if (this != &v_) {
				v = std::move(v_.v);
				i = v_.i;
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

		// TODO: make more efficient
		// same container???
		bool operator==(const vector& _v) const = default;

		explicit operator bool() const
		{
			return i < v.size();
		}
		T operator*() const
		{
			return v[i];
		}
		vector& operator++()
		{
			if (operator bool()) {
				++i;
			}

			return *this;
		}

		// Multi-pass
		vector& reset(size_t i_ = 0)
		{
			i = i_;

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
		return vector<T>(i);
	}

	// Iterable with no elements.
	template<class T>
	struct empty {
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		bool operator==(const empty&) const
		{
			return true;
		}

		explicit operator bool() const noexcept
		{
			return false;
		}
		value_type operator*() const noexcept
		{
			return T{};
		}
		empty& operator++() noexcept
		{
			return *this;
		}
		empty operator++(int) noexcept
		{
			return empty{};
		}
	};

	// Constant iterable: {c, c, c, ...}
	template <class T>
	class constant {
		T c;

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		constant(T c = 0) noexcept
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
		constant operator++(int) noexcept
		{
			return constant(c);
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

		bool operator==(const iota& i) const = default;

		explicit operator bool() const noexcept 
		{ 
			return true; 
		}
		value_type operator*() const noexcept 
		{
			return t; 
		}
		iota& operator++() noexcept
		{
			++t;

			return *this;
		}
		iota operator++(int) noexcept
		{
			iota i{ *this };
			
			operator++();

			return i;
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

		bool operator==(const power& p) const = default;

		explicit operator bool() const noexcept
		{
			return true;
		}
		value_type operator*() const noexcept
		{
			return tn;
		}
		power& operator++() noexcept
		{
			tn *= t;

			return *this;
		}
		power operator++(int) noexcept
		{
			auto p{ *this };
			
			operator++();

			return p;
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

		bool operator==(const factorial& f) const = default;

		explicit operator bool() const noexcept
		{
			return true;
		}
		value_type operator*() const noexcept	
		{
			return t;
		}
		factorial& operator++() noexcept
		{
			t *= n++;

			return *this;
		}
		factorial operator++(int) noexcept
		{
			auto f{ *this };

			operator++();

			return f;
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

		explicit operator bool() const noexcept
		{
			return k <= n;
		}
		value_type operator*() const noexcept
		{
			return nk;
		}
		choose& operator++() noexcept
		{
			if (operator bool()) {
				nk *= n - k;
				++k;
				nk /= k;
			}

			return *this;
		}
		choose operator++(int) noexcept
		{
			auto c{ *this };

			operator++();

			return c;
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

		bool operator==(const pointer& _p) const = default;

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
		pointer operator++(int) noexcept
		{
			auto _p{ *this };

			operator++();

			return _p;
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

		bool operator==(const null_terminated_pointer& _p) const = default;

		explicit operator bool() const noexcept
		{
			return *p != 0;
		}
		value_type operator*() const noexcept
		{
			return *p;
		}
		null_terminated_pointer& operator++() noexcept
		{
			if (operator bool())
				++p;

			return *this;
		}
		null_terminated_pointer operator++(int) noexcept
		{
			auto _p{ *this };

			operator++();

			return _p;
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

		bool operator==(const once& o) const = default;

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
		once operator++(int) noexcept
		{
			auto o{ *this };

			operator++();

			return o;
		}
	};

	// Repeat iterable
	template <input I>
	class repeat {
		I i0, i;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = typename I::value_type;

		repeat(I i) noexcept
			: i0(i), i(i)
		{ }

		bool operator==(const repeat& o) const = default;

		explicit operator bool() const noexcept
		{
			return true;
		}
		value_type operator*() const noexcept
		{
			return *i;
		}
		repeat& operator++() noexcept
		{
			if (i) {
				++i;
			}
			else {
				i = i0;
			}

			return *this;
		}
		repeat operator++(int) noexcept
		{
			auto r{ *this };

			operator++();
	
			return r;
		}
	};

	// Take at most n elements.
	template <input I, class T = typename I::value_type>
	class take {
		I i;
		std::size_t n;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		take(const I& i, std::size_t n)
			: i(i), n(n)
		{ }

		bool operator==(const take& t) const = default;

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
		take operator++(int) noexcept
		{
			auto t{ *this };

			operator++();

			return t;
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
	class concatenate2 {
		I0 i0;
		I1 i1;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		concatenate2() = default;
		concatenate2(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{ }
		concatenate2(const concatenate2&) = default;
		concatenate2& operator=(const concatenate2&) = default;
		~concatenate2() = default;

		bool operator==(const concatenate2& i) const = default;

		explicit operator bool() const
		{
			return i0 || i1;
		}
		value_type operator*() const
		{
			return i0 ? *i0 : *i1;
		}
		concatenate2& operator++()
		{
			if (i0) {
				++i0;
			}
			else {
				++i1;
			}

			return *this;
		}
		concatenate2 operator++(int) noexcept
		{
			auto c{ *this };

			operator++();

			return c;
		}
	};
	template<input I>
	inline auto concatenate(I i)
	{
		return i;
	}
	template<input I, input ...Is>
	inline auto concatenate(I i, Is... is)
	{
		return concatenate2(i, concatenate(is...));
	}

	// Sorted i0 and i1 in order. Equivalent (!< and !>) elements are repeated.
	template <input I0, input I1, class T = std::common_type_t<typename I0::value_type, typename I1::value_type>>
	class merge2 {
		I0 i0;
		I1 i1;
		bool _0; // true use i0, false use i1
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		merge2(const I0& i0, const I1& i1)
			: i0(i0), i1(i1)
		{
			if (i0 && i1) {
				if (*i1 < *i0) {
					_0 = false;
				}
				else { // less or equivalent
					_0 = true;
				}
			}
			else if (i0) {
				_0 = true;
			}
			else {
				_0 = false;
			}
		}

		bool operator==(const merge2& i) const = default;

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
					return _0 ? *i0 : *i1;
				}
			}

			return i0 ? *i0 : *i1;
		}
		merge2& operator++()
		{
			if (i0 && i1) {
				if (*i0 < *i1) {
					++i0;
				}
				else if (*i1 < *i0) {
					++i1;
				}
				else { // equivalent
					if (_0) {
						++i0;
					}
					else {
						++i1;
					}
					_0 = !_0; // switch
				}
			}
			else {
				if (i0) {
					++i0;
					_0 = true;
				}
				else if (i1) {
					++i1;
					_0 = false;
				}
			}
	

			return *this;
		}
		merge2 operator++(int) noexcept
		{
			auto m{ *this };

			operator++();

			return m;
		}
	};
	template<input I>
	inline auto merge(I i)
	{
		return i;
	}
	template<input I, input ...Is>
	inline auto merge(I i, Is... is)
	{
		return merge2(i, merge(is...));
	}

	// f(), ...
	template <class F, class T = std::invoke_result_t<F>>
	class call {
		F f;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		call(F&& f)
			: f(f)
		{ }

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
		F f;
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
		apply(apply&& a)
			: f(a.f), i(std::move(a.i))
		{ }
		apply& operator=(const apply& a)
		{
			if (this != &a) {
				//f = a.f;
				i = a.i;
			}

			return *this;
		}
		apply& operator=(apply&& a)
		{
			if (this != &a) {
				//f = a.f;
				i = std::move(a.i);
			}

			return *this;
		}
		~apply()
		{ }

		bool operator==(const apply& a) const
		{
			return i == a.i;
		}

		explicit operator bool() const
		{
			return i.operator bool();
		}
		value_type operator*() const
		{
			return f(*i);
		}
		apply& operator++() noexcept
		{
			++i;

			return *this;
		}
		apply operator++(int) noexcept
		{
			auto a{ *this };

			operator++();

			return a;
		}
	};

	// TODO: apply(f, *i0, *i1, ...), apply(f, {*++i0, *++i1, ...}), ...

	// Apply a binary operation to elements of two iterable.
	template <class BinOp, input I0, input I1, class T0 = typename I0::value_type, class T1 = typename I1::value_type, class T = std::invoke_result_t<BinOp, T0, T1>>
	class binop {
		BinOp op;
		I0 i0;
		I1 i1;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		binop(const BinOp& op, I0 i0, I1 i1)
			: op(op), i0(i0), i1(i1)
		{ }
		binop(BinOp&& op, I0 i0, I1 i1)
			: op(op), i0(i0), i1(i1)
		{ }
		binop(const binop& o)
			: op(o.op), i0(o.i0), i1(o.i1)
		{ }
		binop(binop&& o) noexcept
			: op(o.op), i0(std::move(o.i0)), i1(std::move(o.i1))
		{ }
		binop& operator=(const binop& o)
		{
			if (this != &o) {
				//op = o.op;
				i0 = o.i0;
				i1 = o.i1;
			}

			return *this;
		}
		binop& operator=(binop&& o)
		{
			if (this != &o) {
				//op = o.op;
				i0 = std::move(o.i0);
				i1 = std::move(o.i1);
			}
		}
		~binop() { }

		bool operator==(const binop& o) const 
		{
			return i0 == o.i0 && i1 == o.i1;
		}

		explicit operator bool() const
		{
			return i0 && i1;
		}
		value_type operator*() const
		{
			return op(*i0, *i1);
		}
		binop& operator++() noexcept
		{
			++i0;
			++i1;

			return *this;
		}
		binop operator++(int) noexcept
		{
			auto b{ *this };

			operator++();

			return b;
		}
	};

	// Elements satisfying predicate.
	template <class P, input I, class T = typename I::value_type>
	class filter {
		P p;
		I i;

		void incr()
		{
			while (i && ++i && !p(*i))
				;
		}
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		filter(const filter& a)
			: p(a.p), i(a.i)
		{
			incr();
		}
		filter(P&& p, const I& i)
			: p(p), i(std::move(i))
		{
			incr();
		}
		filter(filter&& a)
			: p(a.p), i(std::move(a.i))
		{ }
		filter& operator=(const filter& a)
		{
			if (this != &a) {
				//p = a.p;
				i = a.i;
			}

			return *this;
		}
		filter& operator=(filter&& a)
		{
			if (this != &a) {
				//p = a.p;
				i = std::move(a.i);
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
		filter& operator++() noexcept
		{
			incr();

			return *this;
		}
		filter operator++(int) noexcept
		{
			auto f{ *this };

			operator++();

			return f;
		}
	};

	// Stop at first element satisfying predicate.
	template <class P, input I, class T = typename I::value_type>
	class until {
		P p;
		I i;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		until(const P& p, const I& i)
			: p(p), i(i)
		{ }
		until(P&& p, const I& i)
			: p(p), i(i)
		{ }
		until(const until& a)
			: p(a.p), i(a.i)
		{ }
		until(until&& a)
			: p(a.p), i(std::move(a.i))
		{ }
		until& operator=(const until& u)
		{
			if (this != &u) {
				//p = u.p;
				i = u.i;
			}

			return *this;
		}
		until& operator=(until&& u)
		{
			if (this != &u) {
				//p = u.p;
				i = std::move(u.i);
			}

			return *this;
		}
		~until()
		{ }

		bool operator==(const until& u) const
		{
			return i == u.i;
		}

		explicit operator bool() const
		{
			return i && !p(*i);
		}
		value_type operator*() const
		{
			return *i;
		}
		until& operator++() noexcept
		{
			++i;

			return *this;
		}
		until operator++(int) noexcept
		{
			auto u{ *this };

			operator++();

			return u;
		}
	};

	// Right fold: t, op(t, *i), op(op(t, *i), *++i), ...
	template <class BinOp, input I, class T = typename I::value_type>
	class fold {
		BinOp op;
		I i;
		T t;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = T;

		fold(const BinOp& op, const I& i, T t = 0)
			: op(op), i(i), t(t)
		{ }
		fold(BinOp&& op, const I& i, T t = 0)
			: op(op), i(i), t(t)
		{ }
		fold(const fold& f)
			: fold(f.op, f.i, f.t)
		{ }
		fold& operator=(const fold& f)
		{
			if (this != &f) {
				i = f.i;
				t = f.t;
			}

			return *this;
		}
		fold& operator=(fold&& f) noexcept
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
			return i == f.i && t == f.t;
		}

		explicit operator bool() const
		{
			return i.operator bool();
		}
		value_type operator*() const noexcept
		{
			return t;
		}
		fold& operator++() noexcept
		{
			if (i) {
				t = op(t, *i);
				++i;
			}

			return *this;
		}
		fold operator++(int) noexcept
		{
			auto f{ *this };

			operator++();

			return f;
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
		D d;
		I i;
		T t, _t;
		void init()
		{
			if (i) {
				t = *i;
				++i;
				_t = i ? *i : t;
			}
		}
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = U;

		delta(const I& _i, const D& _d = std::minus<T>{})
			: d(_d), i(_i), t{}, _t{}
		{
			init();
		}
		delta(I&& _i, const D& _d = std::minus<T>{})
			: d(_d), i(std::move(_i)), t{}, _t{}
		{
			init();
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
		delta& operator++() noexcept
		{
			if (i) {
				t = *i;
				++i;
			}

			return *this;
		}
		delta operator++(int) noexcept
		{
			auto d{ *this };

			operator++();

			return d;
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

	template<input I, input J>
	class pair {
		I i;
		J j;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<typename I::value_type, typename J::value_type>;

		pair(I i, J j)
			: i(i), j(j)
		{ }

		explicit operator bool() const
		{
			return i and j;
		}
		value_type operator*() const
		{
			return { *i, *j };
		}
		pair& operator++() noexcept
		{
			++i;
			++j;

			return *this;
		}
		pair operator++(int) noexcept
		{
			auto p{ *this };

			operator++();

			return p;
		}
	};

} // namespace fms::iterable

#define FMS_ITERABLE_OPERATOR(X) \
    X(+, std::plus<T>{})        \
    X(-, std::minus<T>{})       \
    X(*, std::multiplies<T>{})  \
    X(/, std::divides<T>{})     \
    X(%, std::modulus<T>{})

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

template<fms::iterable::input I, class T = typename I::value_type>
inline auto operator-(const I& i)
{
	return fms::iterable::constant(T(-1)) * i;
}

template <fms::iterable::input I, fms::iterable::input J>
inline auto operator,(const I& i, const J& j)
{
	return fms::iterable::concatenate2(i, j);
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
template<fms::iterable::input I, class T = typename I::value_type>
inline auto operator<(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u < t; }, i);
}
template<fms::iterable::input I, class T = typename I::value_type>
inline auto operator<=(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u <= t; }, i);
}
template<fms::iterable::input I, class T = typename I::value_type>
inline auto operator>(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u > t; }, i);
}
template<fms::iterable::input I, class T = typename I::value_type>
inline auto operator>=(const I& i, T t)
{
	return fms::iterable::filter([t](T u) { return u >= t; }, i);
}
