// fms_iterable.cpp
#include <cassert>
#include <iostream>
#include "fms_iterable.h"


using namespace fms;

bool test_pointer_ = test_pointer();
bool test_array_i = test_array<int>();
//bool test_iota_ = test_iota();
bool test_null_ = test_null();
bool test_constant_i = test_constant<int>();
bool test_until_ = test_until();
bool test_take_i = test_take<int>();
bool test_scan_ = test_scan();
bool test_when_ = test_when();
bool test_mask_ = test_mask();
bool test_epsilon_d = test_epsilon<double>();
bool test_epsilon_f = test_epsilon<float>();
bool test_factorial_i = test_factorial<int>();
bool test_factorial_d = test_factorial<double>();
bool test_join_ = test_join();

bool test_copy = []() {
	{
		int i[] = { 1,2,3 }, j[3];
		assert(copy(array(i), array(j)));
		assert(equal(array(i), array(j)));
	}

	return true;
}();

/*
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
*/

// quicksort i on pivot p
template<iterable I, class T = typename I::value_type>
auto qsort(T p, I i)
{
	return (i & i < p), unit(p); // , (i & i > p);
}

bool test_qsort = []() {
	{
		int i[] = { 3,2,1,4 };
		auto s = qsort(take(1, array(i)), array(i));
	}

	return true;
}();

int main()
{
	constexpr double eps = std::numeric_limits<double>::epsilon();
	{
		double x = 1;
		auto xn = power(x) / factorial(0.);
		double expx = add(epsilon(xn));
		double ex = exp(x);
		assert(std::fabs(ex - expx) <= eps);
	}
	{
		double x = 1;
		assert(std::fabs(exp(x) - add(epsilon(power(x) / factorial(0.)))) <= eps);
	}
	{
		// filter
		auto e = (iota(0) | [](const auto& i) { return *i % 2; });
		assert(e);
		assert(*e == 1);
		++e;
		assert(e);
		assert(*e == 3);
	}
	{
		auto i = iota(0);
		// mask
		auto o = i & (i % 2);
		assert(o);
		assert(*o == 1);
		++o;
		assert(*o == 3);
	}
	{
		auto i = iota(0);
		// mask
		auto o = i & ~(i % 2);
		assert(o);
		assert(*o == 0);
		++o;
		assert(*o == 2);
	}
	{
		//??? ostream_joiner
		copy(take(3, iota(0)), std::ostream_iterator<int>(std::cout, ", "));
	}

	return 0;
}