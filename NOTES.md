# NOTES

Can we may std::function part of the class signature instead of a member?

Basic operations:
	when(p, i) - filter based on p(i)
	done(p, i) - stop when p(i)
	scan(i) - {{i}, {i, ++i}, ...}
	apply(f, i) - {f(i), f(++i), ...}
	join(i, j, ...) {i, i++, ..., j, j++, ...}
	tuple(i, j, ...) {<i,j,...>, <++i,++j, ...>, ...} 
		???zip
		???stop all end? use done to stop early
	cycle(i) {i, ++i, ..., i, ++i, ...}

iterable:
	!!i -> bool, * and ++ permitted
	*i -> T
	++i -> I&

B
	apply: 
		!!
		*i -> f(i)
		++i

	tuple: 
		!! -> all !!i
		*i -> <*i>
		++i -> <++i>

	join: 
		!!(i,j) -> !!i and !!j
		*(i,j) -> i ? *i : *j
		++(i,j) -> i ? (++i,j) : (i, ++j) 

