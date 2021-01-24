// fms_iterable.cpp
#include <cassert>
#include "fms_iterable.h"

using namespace fms;

//bool test_iota_ = test_iota();
bool test_constant_i = test_constant<int>();
bool test_until_ = test_until();
bool test_take_i = test_take<int>();
bool test_concatenate_ = test_concatenate();

//bool test_iterable_ = test_iterable();
/*


bool test_forward_iterable_ = test_forward_iterable();
bool test_array_i = test_array<int>();
bool test_apply_ = test_apply();
bool test_binop_i = test_binop<int>();
bool test_fold_ = test_fold();
bool test_take_ = test_take<int>();

bool test_last_0 = test_last(take(0, iota(1)));
bool test_last_3 = test_last(take(3, iota(1)));
*/

template<class... Is>
inline auto tuple_incr(Is&&... is)
{
	return [&...is = std::forward<Is>(is)]() {
		std::tuple<Is...>(++is...); //???why is std::tuple needed
	};
}
template<class... Is>
inline auto tuple_star(Is&&... is)
{
	return [...is = std::forward<Is>(is)]() {
		return std::tuple<decltype(*is)...>(*is...); //???why is std::tuple needed
	};
}


int main()
{
	iota i0(0), i1(1);

	auto i = tuple_incr(i0, i1);
	assert(*i0 == 0);
	auto i_star = tuple_star(i0, i1);
	auto i_s = i_star();
	assert(i_s == std::tuple(*i0, *i1));
	i();
	assert(*i0 == 1);

	return 0;
}