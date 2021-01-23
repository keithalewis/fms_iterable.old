# iterable

This header only library uses C++20 to make iterators easier to use by
adding the member function `explicit operator bool() const` instead of using the STL `end()`
notion to delimit ranges. Perhaps a catchier name is `iterabool`.

Iterables allow for infinite (lazy) ranges. This is not a new idea.
Many other languages use something similar, in particular C# where it is used
to make [`LINQ`](https://docs.microsoft.com/en-us/dotnet/csharp/programming-guide/concepts/linq/)
the coolest thing since button shoes.

Using iterables rather than arrays makes C++ more expressive. For example, `sum(epsilon(power(x)/factorial(0)))`
computes `exp(x)` to machine precision using exp(_x_) = &Sigma;<sub>_n_ ≥ 0</sub> _x_<sup>_n_</sup>/_n_!.

A `C` array can be made into an iterable by supplying its size. The library provides for constant
iterables and infinite arithmetic sequences called `iota(t = 0, dt = 1)` to generate `{t, t + dt, ...}`.

Overloads for arithmetic operators provide syntactic sugar. 
For example, `c(a) + c(b) * iota{}` is another way to say `iota(a, b)`. Likewise, `itoa{} * b + a`.

The standard stream oriented functors `apply`,  `filter`, and (right) `fold` create new iterables.
For example `i | i % 2 == 0` is a filter that extracts the even values of `i`. It is equivalent
to `filter(eq(mod(i,2), c(0)), i)`.

Iterables also have `begin()` and `end()` member functions and can be
used with STL routines or in range based for loops. 

Asynchronous programming becomes trivial using coroutines.
If `i` is iterable then
```
auto coro(I i) 
{
	while (i) {
		co_yield *i;
		++i;
	}
}
```
is a generator for the iterable.

As you will see when you peruse the code, most functions involving iterables have a natural and pleasing implmentation.
