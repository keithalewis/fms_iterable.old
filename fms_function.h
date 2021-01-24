// fms_function.h - functions for iterables
#pragma once
#include "fms_iterable.h"

namespace fms {

#pragma region all

	// every element is convertible to true
	template<iterable I>
	inline constexpr bool all(I i)
	{
		return !!i ? (*i and all(++i)) : true;
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
		return !!i ? (*i or any(++i)) : false;
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

	// STL end
	//!!! specialize for random access iterable
	template<iterable I>
	inline I end(I i)
	{
		for (; i; ++i) ; // nothing

		return i;
	}
	// last before end
	template<iterable I>
	inline I last(I i)
	{
		for (I i_ = i; ++i_; i = i_) ; // nothing

		return i;
	}


#pragma endregion any
	template<iterable I>
	inline I skip(size_t n, I i)
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
		for (; i; ++i) { ++n; }

		return n;
	}
	// length alias
	template<iterable I>
	inline size_t size(I i, size_t n = 0)
	{
		return length(i, n);
	}

	// last iterable before end
	template<iterable I>
	inline I last(I i)
	{
		I i_(i);

		while (++i) {
			++i_;
		}

		return i_;
	}

#ifdef _DEBUG

	template<iterable I>
	inline bool test_last(I i)
	{
		{
			assert(++last(i) == i.end());
		}

		return true;
	}

#endif // _DEBUG


}
