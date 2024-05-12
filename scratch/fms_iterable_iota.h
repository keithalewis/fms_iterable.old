// fms_iterable_iota.h - 0, 1, 2, ...
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <limits>
#include "fms_iterable.h"

namespace fms::iterable {

	// [b, b + 1, ..., e)
	template<typename T>
	class iota {
		T b, e;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = std::remove_cv_t<T>;
		//using reference = T&;

		constexpr iota(T b = 0, T e = std::numeric_limits<T>::max())
			: b(b), e(e)
		{ }
		constexpr iota(const iota&) = default;
		constexpr iota& operator=(const iota&) = default;
		constexpr ~iota() = default;

		constexpr bool operator==(const iota&) const = default;
		
		constexpr iota begin() const
		{
			return iota(b, e);
		}
		constexpr iota end() const
		{
			return iota(e, e);
		}

		constexpr explicit operator bool() const
		{
			return b < e;
		}
		constexpr value_type operator*() const
		{
			return b;
		}
		constexpr iota& operator++()
		{
			if (b < e) {
				++b;
			}

			return *this;
		}
		constexpr iota operator++(int)
		{
			auto i = *this;

			operator++();

			return i;
		}
#ifdef _DEBUG

		static int test()
		{
			{
				static_assert(compare(iota(0, 3), iota(0, 3)) == 0);
				static_assert(compare(iota(0, 2), iota(0, 3)) < 0);
				static_assert(compare(iota(0, 3), iota(0, 2)) > 0);
				static_assert(compare(iota(1, 3), iota(2, 4)) < 0);
				static_assert(compare(iota(2, 3), iota(1, 2)) > 0);
				static_assert(compare(iota(0, 3), iota(2, 3)) < 0);
			}
			{
				static_assert(equal(iota(0, 3), iota(0, 3)));
				static_assert(!equal(iota(0, 2), iota(0, 3)));
			}
			{
				static_assert(compare(iota(0, 3), iota<float>(0, 3)) == 0);
				static_assert(compare(iota<double>(0, 2), iota(0, 3)) < 0);
				static_assert(compare(iota<float>(0, 3), iota<double>(0, 2)) > 0);
				static_assert(compare(iota<float>(0, 3), iota<double>(0, 3)) == 0);
			}
			{
				constexpr auto i = iota<T>{};
				static_assert(i);
				static_assert(i == i.begin());
				static_assert(i != i.end());
				constexpr auto i2(i);
				static_assert(i2);
				static_assert(i2 == i);
				constexpr auto i3 = i2;
				assert(i3);
				assert(!(i3 != i2));

				static_assert(*i == 0);
			}
			{
				iota<T> i(0, 3);
				assert(i);
				assert(*i == 0);
				++i;
				assert(i);
				assert(*i == 1);
				++i;
				assert(i);
				assert(*i++ == 2);
				assert(!i);
			}
			{
				T t(0);
				for (auto i : iota<T>(0, 3)) {
					assert(i == t);
					++t;
				}
				assert(t == 3);
			}
			{
				T t(0);
				iota<T> i(0, 3);
				while (i++) {
					++t;
				}
				assert(t == 3);
				assert(i == i.end());
			}

			return 0;
		}

#endif // _DEBUG
	};

} // namespace fms::iterable
