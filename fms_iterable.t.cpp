// fms_iterable.t.cpp - test fms::iterable
#include "fms_time.h"
#include "fms_iterable.h"
#include <cassert>
#include <cmath>
#include <vector>
// #include "fms_time.h"
// #include "tmx_math_limits.h"

using namespace fms::iterable;

int test_interval() {
	{
		std::vector v{ 1, 2, 3 };
		interval c(v.begin(), v.end());
		auto c2(c);
		assert(c == c2);
		// c = c2; // cannot reseat const reference
		assert(!(c2 != c));

		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(!c);
	}
	{
		std::vector v{ 1, 2, 3 };
		interval c(v.begin(), v.end());
		int i = 1;
		for (auto ci : c) {
			assert(i == ci);
			++i;
		}
	}
	{
		std::vector v{ 1, 2, 3 };
		interval c(v.begin(), v.end());
		assert(equal(c, take(iota(1), 3)));
	}
	{
		std::vector v{ 1, 2, 3 };
		auto c = make_interval(v);
		assert(equal(c, take(iota(1), 3)));
	}

	return 0;
}

int test_list() {
	{
		list c({ 1, 2, 3 });
		assert(c);
		auto c2(c);
		assert(c == c2);
		assert(equal(c, c2));
		c = c2;
		assert(!(c2 != c));
		++c2;
		assert(c2 != c);

		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(!c);
	}
	{
		list c({ 1, 2, 3 });
		assert(!equal(c, iota(1)));
		assert(equal(c, take(iota(1), 3)));
	}
	{
		list c({ 1, 2, 3 });
		int i = 1;
		for (auto ci : c) {
			assert(i == ci);
			++i;
		}
	}

	return 0;
}

int test_constant() {
	constant c(1);
	constant c2(c);
	c = c2;
	assert(c == c2);
	assert(!(c2 != c));

	assert(c);
	assert(*c == 1);
	++c;
	assert(c);
	assert(*c == 1);

	auto cc = constant(1.) * constant(2.);

	return 0;
}

int test_choose() {
	{
		list l({ 1, 3, 3, 1 });
		choose c(3);
		assert(equal(l, c));
	}
	return 0;
}

int test_once() {
	{
		once o(1);
		once o2(o);
		o = o2;
		assert(o == o2);
		assert(!(o2 != o));

		assert(o);
		assert(*o == 1);
		++o;
		assert(!o);
	}

	return 0;
}

int test_repeat() {
	{
		repeat r(once(1));
		assert(equal(take(r, 3), take(constant(1), 3)));
	}

	return 0;
}

int test_iota() {
	{
		iota i(2);
		assert(i);
		auto i2(i);
		i = i2;
		assert(i == i2);
		assert(!(i2 != i));

		assert(*i == 2);
		++i;
		assert(i);
		assert(*i == 3);
	}

	return 0;
}

int test_skip() {
	{
		iota<int> i;
		i = drop(i, 2);
		assert(*i != 0);
		assert(*i == 2);
	}
	{
		iota<int> i;
		auto j = drop(i, 2);
		assert(*i == 0);
		assert(*j == 2);
	}

	return 0;
}

int test_power() {
	{
		power p(2);
		assert(p);
		auto p2(p);
		p = p2;
		assert(p == p2);
		assert(!(p2 != p));

		assert(*p == 1);
		++p;
		assert(p);
		assert(*p == 2);
		++p;
		assert(p);
		assert(*p == 4);
		++p;
		assert(p);
	}

	return 0;
}

int test_factorial() {
	{
		factorial f;
		assert(f);
		auto f2(f);
		assert(f == f2);
		f = f2;
		assert(!(f2 != f));

		assert(*f == 1);
		assert(*++f == 1);
		assert(*++f == 2);
		assert(*++f == 6);
	}

	return 0;
}

int test_apply() {
	{
		apply a([](int x) { return 0.5 * x; }, iota<int> {});
		auto a2(a);
		assert(a == a2);
		//a = a2;
		assert(!(a2 != a));

		assert(a);
		assert(*a == 0.);
		++a;
		assert(a);
		assert(*a == 0.5);
		++a;
		assert(a);
		assert(*a == 1.);
	}
	{
		apply a([](int x) { return 0.5 * x; }, iota<int> {});
		apply a2([](int x) { return 0.5 * x; }, iota<int> {});
		//assert(a != a2);
		apply a3(a2);
		assert(a3);
		assert(*a3 == 0.);
		++a3;
		assert(a3);
		assert(*a3 == 0.5);
		++a3;
		assert(a3);
		assert(*a3 == 1.);

	}

	return 0;
}

int test_until() {
	{
		until a(
			[](double x) { return x <= std::numeric_limits<double>::epsilon(); },
			power(0.5));
		auto a2(a);
		assert(a == a2);
		a = a2;
		assert(!(a2 != a));

		auto len = length(a);
		assert(len == 52);
	}

	return 0;
}

int test_filter() {
	{
		filter a([](int i) { return i % 2; }, iota<int> {});
		//auto a2(a);
		//assert(a == a2);

		assert(a);
		assert(*a == 1);
		++a;
		assert(a);
		assert(*a == 3);
		++a;
		assert(a);
		assert(*a == 5);
	}
	{
		iota<int> i;
		auto j = (i > 2);
		assert(j);
		assert(*j == 3);
		++j;
		assert(*j == 4);
	}
	{
		/*
		iota<int> i;
		auto j = ((i >= 2) <= 4);
		assert(*j == 2);
		assert(*++j == 3);
		assert(*++j == 4);
		assert(!++j);
		*/
	}

	return 0;
}

int test_fold() {
	{
		fold f(std::plus<int> {}, iota<int> {});
		auto f2(f);
		assert(f == f2);
		f = f2;
		assert(!(f2 != f));

		assert(f);
		assert(*f == 0);
		++f;
		assert(f);
		assert(*f == 0);
		++f;
		assert(f);
		assert(*f == 1);
		++f;
		assert(f);
		assert(*f == 3);
	}
	{
		fold f(std::plus<int> {}, iota<int> {});
		fold g(std::plus<int> {}, iota<int> {});
		assert(f == g);
		++g;
		assert(f != g);
	}
	{
		fold f(std::multiplies<int> {}, iota<int>(1), 1);
		// 1 * 1 * 2 * 3
		f = drop(f, 3);
		assert(*f == 6);
	}
	{
		fold f(std::multiplies<int> {}, iota<int>(1), 1);
		// 1 * 1 * 2 * 3
		auto f_ = back(take(f, 4));
		assert(*f_ == 6);
	}

	return 0;
}

int test_pointer() {
	int i[] = { 1, 2, 3 };
	pointer p(i);
	pointer p2(p);
	assert(p == p2);
	p = p2;
	assert(!(p2 != p));

	assert(p);
	assert(*p == 1);
	++p;
	assert(*p == 2);
	++p;
	assert(*p == 3);
	++p; // *p is undefined

	return 0;
}

int test_zero_pointer() {
	{
		int i[] = { 1, 2, 0 };
		null_terminated_pointer p(i);
		auto i2(i);
		assert(i == i2);
		// i = i2; // int[3] not assignable
		// assert(!(i2 != i));

		assert(p);
		assert(*p == 1);
		++p;
		assert(p);
		assert(*p == 2);
		++p;
		assert(!p);
	}
	{
		char i[] = "ab";
		null_terminated_pointer s(i);
		assert(s);
		assert(*s == 'a');
		++s;
		assert(*s == 'b');
		++s;
		assert(!s);
	}

	return 0;
}

int test_take() {
	int i[] = { 1, 2, 3 };
	{
		pointer p(i);
		take<pointer<int>, int> t(p, 3);
		auto t2(t);
		t = t2;
		assert(t == t2);
		assert(!(t2 != t));

		assert(t);
		assert(*t == 1);
		++t;
		assert(t);
		assert(*t == 2);
		++t;
		assert(*t == 3);
		++t;
		assert(!t);
	}
	{
		auto t = array(i);
		assert(t);
		assert(*t == 1);
		++t;
		assert(t);
		assert(*t == 2);
		++t;
		assert(*t == 3);
		++t;
		assert(!t);
	}
	{
		auto t = array(i);
		assert(equal(t, t));
		assert(length(t) == 3);
		assert(length(drop(t, 1), length(t)) == 5);
	}

	return 0;
}

int test_concatenate() {
	int i[] = { 1, 2, 3 };
	int j[] = { 4, 5, 6 };
	{
		auto p = array(i);
		auto q = array(j);
		auto c = concatenate(p, q);
		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(c);
		assert(*c == 4);
		++c;
		assert(*c == 5);
		++c;
		assert(*c == 6);
		++c;
		assert(!c);
	}
	{
		auto p = array(i);
		auto q = array(j);
		auto c = concatenate(p, q);
		auto c2(c);
		assert(c == c2);
		c2 = c;
		assert(!(c2 != c));

		assert(*c2 == 1);
		++c2;
		assert(c2);
		assert(*c2 == 2);
		++c2;
		assert(*c2 == 3);
		++c2;
		assert(c2);
		assert(*c2 == 4);
		++c2;
		assert(*c2 == 5);
		++c2;
		assert(*c2 == 6);
		++c2;
		assert(!c2);
	}
	{
		vector v({ 1,2,3 });
		const auto v_ = concatenate(v, empty<int>{});
		assert(equal(v, v_));
		const auto _v = concatenate(empty<int>{}, v);
		assert(equal(v, _v));
	}
	{
		vector v1({ 1,2 }), v2({ 3, 4, 5 }), v3({ 6, 7, 8, 9 });
		const auto v = concatenate(v1, v2, v3);
		assert(equal(v, list({ 1, 2, 3, 4, 5, 6, 7, 8, 9 })));

		const auto v_ = v;
		assert(equal(v, v_));
		assert(equal(v, take(iota(1), 9)));
	}

	return 0;
}

int test_merge() {
	{
		int i[] = { 1, 3, 5 };
		int j[] = { 2, 4, 6 };
		auto p = array(i);
		auto q = array(j);
		auto c = merge(p, q);
		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(c);
		assert(*c == 4);
		++c;
		assert(*c == 5);
		++c;
		assert(*c == 6);
		++c;
		assert(!c);
	}
	{
		int i[] = { 1, 3, 5 };
		int j[] = { 2, 4, 6 };
		auto c = merge(array(i), array(j));
		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(c);
		assert(*c == 4);
		++c;
		assert(*c == 5);
		++c;
		assert(*c == 6);
		++c;
		assert(!c);
	}
	{
		int i[] = { 1, 2 };
		int j[] = { 2, 3 };
		auto c = merge(array(i), array(j));
		assert(equal(c, list({ 1, 2, 2, 3 })));
	}
	{
		int i[] = { 1, 2, 2 };
		int j[] = { 2, 2, 3 };
		auto c = merge(array(i), array(j));
		assert(equal(c, list({ 1, 2, 2, 2, 2, 3 })));
	}
	{
		auto i = iota(1);
		auto j = constant(2) * i; // 2, 4, 6, 8, ...
		auto k = i + j;		      // 3, 6, 9, 12, ...
		auto l = merge(j, k);
		assert(equal(take(l, 6), list({ 2, 3, 4, 6, 6, 8 })));
	}
	{
		vector v({ 1,2,3 });
		const auto v_ = merge(v, empty<int>{});
		assert(equal(v, v_));
		const auto _v = merge(empty<int>{}, v);
		assert(equal(v, _v));
	}

	return 0;
}

int test_vector() {
	{
		int i[] = { 1, 2, 3 };
		{
			auto p = array(i);
			auto c = make_vector(p);
			assert(c);
			auto c2{ c };
			assert(c == c2);
			c = c2;
			assert(!(c2 != c));

			auto cc = make_vector(c);
			assert(cc);
			assert(equal(cc, c));

			vector ccc(cc);
			assert(equal(ccc, c));

			assert(*c == 1);
			++c;
			assert(c);
			assert(*c == 2);
			++c;
			assert(*c == 3);
			++c;
			assert(!c);

		}
		{
			auto c = vector(3, i);
			assert(equal(c, take(iota(1), 3)));
		}
		{
			auto c = vector(3, i);
			vector v(c);
			assert(equal(v, c));
		}

	}

	return 0;
}

int test_delta() {
	{
		delta d(power<int>(2));
		auto d2(d);
		assert(d == d2);
		d = d2;
		assert(!(d2 != d));

		assert(d);
		assert(*d == 2 - 1);
		++d;
		assert(*d == 4 - 2);
	}
	{
		delta d(power<int>(2));
		auto _d = -d;

		assert(_d);
		assert(*_d == -1);
		++_d;
		assert(*_d == -2);
	}
	{
		auto _d = -delta(power<int>(2));

		assert(_d);
		assert(*_d == -1);
		++_d;
		assert(*_d == -2);
	}

	return 0;
}

int test_call() {
	{
		call c([]() { static int i = 0;  return i++; });

		assert(equal(take(c, 3), list({ 0, 1, 2 })));
	}

	return 0;
}

int test_exp() {
	const auto eps = [](double x) { return x + 1 == 1; };
	double x = 1;
	{
		// exponential(x) = sum x^n/n!
		auto expx = sum(until(eps, power(x) / factorial()));
		double exp1 = std::exp(1.);
		assert(std::fabs(expx - exp1) <= 5e-16);
	}
	{
		auto expx = until(eps, power(x) / factorial());
		auto tx = fms::time([&]() { sum(expx); }, 10'000);
		auto t1 = fms::time([&]() { std::exp(1.); }, 10'000'000);
		//t = fms::time([&]() { sum(expx, 0, std::execution::seq); });
		auto q = 1000 * tx / t1;
		assert(q != 0);
	}

	return 0;
}

int main()
{
	test_interval();
	test_list();
	test_constant();
	test_choose();
	test_once();
	test_repeat();
	test_iota();
	test_skip();
	test_power();
	test_factorial();
	test_apply();
	test_until();
	test_filter();
	test_fold();
	test_pointer();
	test_zero_pointer();
	test_take();
	test_concatenate();
	test_merge();
	test_vector();
	test_delta();
	test_call();
	test_exp();

	return 0;
}
