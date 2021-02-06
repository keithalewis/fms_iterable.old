# NOTES

Can we make std::function part of the class signature instead of a member?

iterable: sequence of homogeneous types

Basic operations:
	i : {t0, t1, ...}
	!i => i == {}
	*i => !i ? bot : t0
	+i => {t1, t2, ...} (+{} => {})
	// !i <=> *i != bot
	//begin(i) => i
	end(i) => {} // while(!!i) +i; ???execute for side effects
	row(i0, i1, ...) => {{t00, t01, ...}, ??? lift
	                      {t10, t11,...}...} iterable of iterables
	col(i0, i1, ...) => {{t00, t10, ...}, ??? tuck
	                     {t01, t11, ...}...} 
	mow({i0}, {i1}, ...) => i0, i1, ...   ??? flat
	map(f, i0) => {f(t0), f(t1), ...}
	scan(i) => {{t0}, {t0, t1}, ...}

	// advance until end or p
	I next(P p, I i) => !i or p(i) ? i : next(p, +i)
	end(I i) = next(false, i)

	
	// stop when predicate holds
	upto(P p, I i) {
		! => !i or p(i)
		* => *i
		+ => +i
	}
	// filter based on predicate
	when(P p, I i) {
		i = next(p, i)
		! => !i
		* => *i
		+ => i = next(p, +i)
	}

	// end or first false
	I all(I i) => !i or !*i ? i : all(+i)
	I all(P t, I i) => !i or !p(i) ? i : all(p, +i)
	I all(P t, I i) => !i ? i : (!p(i) ? i : all(p, +i))
	// end or first true
	I any(I i) => !i or *i ? i : any(+i)
	I any(P p, I i) => !i or p(i) ? i : any(p, +i)

	I until(P p, I i) => !i or p(i)  ? i : until(p, +i) // until(i >= t, i)
	I while(P p, I i) => !i or !p(i) ? i : while(p, +i) // while(i < t, i)
	until(p, i) = while(!p, i)
	all(I i) = while(*i, i)
	any(I i) = until(*i, i)

	// filter based on predicate
	I when(P p, I i) => !i ? i : p(i) ? i : when(p, +i)
	// stop when predicate is true
	I upto(P p, I i) => !i ? i : p(i) ? end(i) : upto(p, +i)

	join(i0, i1) => {t00, t01, ..., t10, t11, ...)
	when(p, i) - filter based on p(i)
	mask(m, i) - when(!!m, i)
	done(p, i) - stop when p(i), 
	apply(f, i) - {f(i), f(++i), ...}
	join(i, j, ...) {i, i++, ..., j, j++, ...}
	tuple(i, j, ...) {<i,j,...>, <++i,++j, ...>, ...} 
		???zip
		???stop all end? use done to stop early
	cycle(i) {i0, i1, ..., i0, i1, ...}

iterable<I,T>
	!!i -> bool
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

