// fms_time.h - Time a function call.
#pragma once
#include <chrono>

namespace fms {

	// Time n calls to f.
	template<class F, class U = std::chrono::milliseconds>
	inline auto time(F&& f, std::size_t n = 1)
	{
		auto start = std::chrono::high_resolution_clock::now();
		while (n--) f();
		auto stop = std::chrono::high_resolution_clock::now();

		return std::chrono::duration_cast<U>(stop - start).count();
	}

} // namespace fms
