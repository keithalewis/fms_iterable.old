# NOTES

iterable:
	!!i -> bool, * and ++ permitted
	*i -> T
	++i -> I&

Basic operations: 
	generate(f): 
		true
		*i -> f()
		++i -> noop

	truncate(p,i):
		p(i)
		*i -> *i
		++i -> ++i

	filter(p, i):
		true
		*i -> *i
		++ -> while !p(i) ++i

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

auto count = [](){static i = 0; return i++;};
auto constant = [k]() { static k0 = k; return k0; }:
auto incr = [](auto& i) { return ++i; };
auto boolean [p](const auto& i) { static P p; return p(i); };
auto value [v](const auto& i) { return v(i); };

template<iterable I, typename... Args>
inline auto filter(Args&&... args)
{
	return [...args = std::forward<Args>(args)](const I& i)
	{
		...args, ..., i
	};
}

template<class OpBool, OpEq, OpStar, OpIncr>
class iterable {
	explicit operator bool() const
	{
		return op_bool(i);
	}
	struct last {
		bool operator==(const iterable& i) const
		{
			return !i;
		}
	}
	auto end()
	{

	}
	value_type operator*() const
	{
		return op_star(i);
	}
	iterable& operator++()
	{
		op_incr(i);

		return *this;
	}
}

inline take(size_t n, const I& i)
{
	return iterable<Count<n>,OpEq,...>(i);
}

template<iterable I>
class counted : public I {
	size_t n;
	counted(const I& i)
		: I(i), n(0)
	{ }
	counted& operator++()
	{
		++n;
		I::operator++();

		return *this;
	}
}

// truncate an iterator
template<class P, iterable I>
class until : public I {
	P p;
	explicit operator bool() const
	{
		return !p(*this);
	}
};