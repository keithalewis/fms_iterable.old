# iterable

Maybe this library should be called `iterabool`.

This header only library uses C++20 to make iterators easier to use by
adding the member function `explicit operator bool() const`. This is not a new idea.
Many other languages use something similar, in particular C# where it is used
to make [`LINQ`](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/concepts/linq/)
the coolest thing since button shoes.

I do a lot of numerical programming and I work very hard to be lazy. Using streams rather
than arrays makes C++ much more expressive. For example, `sum(epsilon(power(x)/factorial()))`
computes `exp(x)` to machine precision using exp(x) = &Sigma;<sub>n >= 0</sub> x<sup>n</sup>/n!.

This library supplies the usual stream functions like `apply`, (right) `fold`, `mask`, and `filter`
and overloads some operators for syntactic sugar. For example, 
`mask(apply([](auto i) { return i <= 2; } iota()), iota()))`, or equivalently 
`filter([](auto i) { return i <= 2; }, iota())`,
returns the elements `{0, 1, ...}` that are less than or equal to 2. 
It can also be written `iota() | iota() <= 2` where `|` is read _given_.

The global `begin()` and `end()` functions can be
used with STL routines or in range based for loops. The `end()` function returns
a class called `empty`. The global function `operator==(const S& s, const empty<S>&)`
asks if the iterabool `s` is done by calling `!s.operator bool()`.
Since `empty<S>` publicly inherits from `S` there is no need for `range` library
gyrations where end sentinals can have a different type than the iterator.

The beauty part of this is asynchronous programming becomes trivial using coroutines.
If `s` is iterabool then
```
auto coro(S s) {
	while (s) {
		co_yield *s;
		++s;
	}
}
```
leverages compiler writers' handiwork to create multitasking code.