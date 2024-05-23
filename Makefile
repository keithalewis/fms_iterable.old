CXXFLAGS = -Wall -g -std=gnu++20 -D_DEBUG -Wno-unknown-pragmas
SOURCES = fms_iterable.t.cpp

fms_iterable.t: $(SOURCES)

.PHONY : clean test

test: ./fms_iterable.t
	./fms_iterable.t

check: $(SOURCES)
	clang-tidy -checks=cert-* --warnings-as-errors=* $(SOURCES)

clean:
	-rm -f fms_iterable.t
