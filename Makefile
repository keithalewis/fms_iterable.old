CXXFLAGS = -Wall -g -std=gnu++20 -D_DEBUG -Wno-unknown-pragmas
SOURCES = fms_iterable.cpp

fms_iterable: $(SOURCES)

.PHONY : clean test

test: ./fms_iterable
	./fms_iterable

check: $(SOURCES)
	clang-tidy -checks=cert-* --warnings-as-errors=* $(SOURCES)

clean:
	-rm fms_iterable
