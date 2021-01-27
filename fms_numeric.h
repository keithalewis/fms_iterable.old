// fms_numeric.h - iterators for numerical types
#pragma once
#include <compare>
#include "fms_iterable.h"

namespace fms {
#pragma region constant

	template<typename T>
	class constant {
		T t;
	public:
		// random access
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
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
	// arithmetic series
	// {t, t + 1, ... }
	template<typename T>
	class iota {
		T t;
	public:
		//??? random access
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
#pragma region power

	// geometric series
	// {tn, tn*t, tn*t^2, ...}
	template<typename T>
	class power {
		T t, tn;
	public:
		//??? random access
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		power(T t = 1, T tn = 1)
			: t(t), tn(tn)
		{ }
		power(const power&) = default;
		power& operator=(const power&) = default;
		~power()
		{ }

		bool operator==(const power& i) const
		{
			return t == i.t and tn == i.tn;
		}
		bool operator!=(const power& i) const
		{
			return !operator==(i);
		}
		auto end() const
		{
			return power(std::numeric_limits<T>::max());
		}

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

#pragma endregion power

#pragma region factorial

	// 1, 1, 1*2, 1*2*3, ...
	template<class T>
	class factorial {
		T n, n_;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		factorial(T n = 0, T n_ = 1)
			: n(n), n_(n_)
		{ }
		factorial(const factorial&) = default;
		factorial& operator=(const factorial&) = default;
		~factorial()
		{ }

		auto operator<=>(const factorial&) const = default;

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return n_;
		}
		factorial& operator++()
		{
			n_ *= ++n;

			return *this;
		}
	};

#ifdef _DEBUG

	template<class T>
	inline bool test_factorial()
	{
		{
			factorial<T> f;
			assert(f);
			assert(*f == 1);
			++f;
			assert(f);
			assert(*f == 1);
			++f;
			assert(f);
			assert(*f == 2);
			++f;
			assert(f);
			assert(*f == 6);
			++f;
		}

		return true;
	}

#endif // _DEBUG

#pragma endregion factorial

#pragma region pochhammer

	// Pochhammer symbol: x(x + k)(x + 2k)...
	// Use k = 1 for rising, k = -1 for falling.
	template<class T>
	class pochhammer {
		T x, k, x_, k_;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;

		pochhammer(T x, T k = 1)
			: x(x), k(k), x_(x), k_(0)
		{ }
		pochhammer(const pochhammer&) = default;
		pochhammer& operator=(const pochhammer&) = default;
		~pochhammer()
		{ }

		auto operator<=>(const pochhammer&) const = default;

		explicit operator bool() const
		{
			return true;
		}
		value_type operator*() const
		{
			return x_;
		}
		pochhammer& operator++()
		{
			k_ += k;
			x_ *= (x + k_);

			return *this;
		}
	};

#pragma endregion pochhammer
}