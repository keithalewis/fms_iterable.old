# NOTES

Can we make std::function part of the class signature instead of a member?

Basic operations:
	i - {i0, i1, ...}
	I all(I i) { return !i ? i : any(++i) }
	when(p, i) - filter based on p(i)
	mask(m, i) - when(!!m, i)
	done(p, i) - stop when p(i), 
	scan(i) - {{i}, {i, ++i}, ...}
	apply(f, i) - {f(i), f(++i), ...}
	join(i, j, ...) {i, i++, ..., j, j++, ...}
	tuple(i, j, ...) {<i,j,...>, <++i,++j, ...>, ...} 
		???zip
		???stop all end? use done to stop early
	cycle(i) {i0, i1, ..., i0, i1, ...}

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

