// fms_iterable.cpp
#include <cassert>
#include <iostream>
#include "fms_iterable_iota.h"
//#include "fms_function.h"

using namespace fms;

int test_ptr = iterable::ptr_test();
int test_iota_i = iterable::iota<int>::test();
//int test_iota_f = iterable::iota<float>::test();
//int test_iota_d = iterable::iota<double>::test();

int main()
{
	return 0;
}

/*
int test_span_i = span<const int*>::test();
int test_span_f = span<const float*>::test();
int test_span_d = span<const double*>::test();

int test_constant_i = constant<int>::test();

int test_array_i = array<int>::test();
int test_array_f = array<float>::test();
int test_array_d = array<double>::test();
*/

//int test_all = all_test();

/*
int test_pointer_ = test_pointer();
int test_list_ = test_list();
int test_null_ = test_null();
int test_constant_i = test_constant<int>();
int test_until_ = test_until();
int test_take_i = test_take<int>();
int test_scan_ = test_scan();
int test_when_ = test_when();
int test_mask_ = test_mask();
int test_epsilon_d = test_epsilon<double>();
int test_epsilon_f = test_epsilon<float>();
int test_factorial_i = test_factorial<int>();
int test_factorial_d = test_factorial<double>();
int test_join_ = test_join();

int test_copy = []() {
	{
		int i[] = { 1,2,3 }, j[3];
		assert(copy(array(i), array(j)));
		assert(equal(array(i), array(j)));
	}

	return true;
}();


template<class... Is>
inline constexpr auto make_lift(Is&&... is)
	return[...is = std::reference_wrapper<Is>{ static_cast<Is&&>(is) }](auto&& f) mutable -> decltype(auto)
	{
		return decltype(f)(f)(is.type...);
	};
}

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

// quicksort i on pivot first element
/*
template<iterable I, class T = typename I::value_type>
struct quicksort : public I {
	struct delay {
		auto get() {
			struct result {
				operator I()
				{
					return delay->get_I();
				}
				delay* delay;
			};
			return result(*this);
		}
	};
};

int test_qsort = []() {
	{
		int i[] = { 3,2,1,4 };
		//auto s = qsort(array(i));
		//copy(s, std::ostream_iterator<int>(std::cout, ", "));
	}

	return true;
}();


template<class... Is>
inline constexpr auto make_lift(Is&&... is) {
	return [&...is = std::forward<Is>(is)](auto&& f) mutable -> decltype(auto)
	{
		return f(is...);
	};
}

template<class I>
inline constexpr void incr(I& i)
{
	++i;
}
template<class I, class... Is>
inline constexpr void incr(I& i, Is&... is)
{
	++i;
	incr(is...);
}
*/

