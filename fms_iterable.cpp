// fms_iterable.cpp
#include <cassert>
#include "fms_iterable.h"

using namespace fms;

bool test_array_i = test_array<int>();
bool test_constant_i = test_constant<int>();
bool test_apply_ = test_apply();
bool test_binop_i = test_binop<int>();
bool test_take_ = test_take<int>();

int main()
{

	return 0;
}