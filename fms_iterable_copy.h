// fms_iterable_copy.h - copy input to output iterable
#pragma once
#include "fms_iterable.h"

namespace fms::iterable {

	template<input_iterable I, output_iterable O>
	inline O copy(I i, O o)
	{
		while (i) {
			*o++ = *i++;
		}

		return o;
	}

} // namespace fms::iterable
