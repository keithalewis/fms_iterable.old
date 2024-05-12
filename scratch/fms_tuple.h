// fms_tuple.h - iterable tuples
// ??? zip
#include <functional>
#include <tuple>
#include <type_traits>
#include "fms_iterable.h"

namespace fms {

	// capture iterator references
	template<class... Is>
	inline constexpr auto capture(Is&&... is) {
		return[&...is = std::forward<Is>(is)](auto&& f) mutable -> decltype(auto)
		{
			return f(is...);
		};
	}

	template<iterable... Is>
	class col {
		std::invoke_result_t<decltype(capture<Is...>), Is&&...> c;
	public:
		col(Is&&... is)
			: c(capture(is...))
		{ }
	};


	template<iterable I, iterable J>
	class mow {
		I i;
		J j;
	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::common_type_t<typename I::value_type, typename J::value_type>;

		mow(const I& i, const J& j)
			: i(i), j(j)
		{ }
		explicit operator bool() const
		{
			return i and j;
		}
		auto end() const
		{
			return mow(i.end(), j.end());
		}
		value_type operator*() const
		{
			return i ? *i : *j;
		}
		mow& operator++()
		{
			i ? ++i : ++j;

			return *this;
		}
	};

}
