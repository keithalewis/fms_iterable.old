// fms_iterable_copy.h - copy input to output iterable
#pragma once
#include "fms_iterable.h"

namespace fms::iterable {

	template<forward_iterable I, forward_iterable O>
	constexpr O copy(I i, O o)
	{
		while (i and o) {
			*o++ = *i++;
		}

		return o;
	}

#ifdef _DEBUG

	inline int copy_test()
	{
		{
			static int i[] = { 0, 1, 2 };
			static int j[3] = { 3, 3, 3 };
			auto ii = ptr(i);
			auto jj = ptr(j);
			//auto kk =  iterable::copy(ii, jj);
			//static_assert(j[0] == 0);
		}

		return 0;
	}

#endif // _DEBUG

} // namespace fms::iterable
