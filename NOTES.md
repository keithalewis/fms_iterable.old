# NOTES

Basic operations: 
	generate(f): void f(void) -> {f(), f(), ...}
	truncate(p,i): p(i) ? i : end
	filter(p, i): p(i) ? i : filter(p, ++i)
	apply: {f(i), f(++i), ...}
	tuple: {<i, j,...>, <++i, ++j, ...>, ...}
	join: {i, ++i, ..., j, ++j, }

template<iterable... is>
void tuple_incr(iterable&... is)
{
	++...is;
}

class generate {
	size_t n;
	F f;
	generate(F f)
		: n(0), f(f)
	{ }
	explicit operator bool() const
	{
		return true;
	}
	value_type operator*() const
	{
		return f();
	}
	generate& operator++()
	{
		++n;

		return *this;
	}
}

template<class T>
struct count {
	T t;
	count(T t) : t(t) { }
	T operator()()
	{
		return t++;
	}
}

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