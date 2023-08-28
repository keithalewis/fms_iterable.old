CXXFLAGS = -Wall -g -std=gnu++20 -D_DEBUG -Wno-unknown-pragmas

fms_iterable: fms_iterable.cpp 

.PHONY : clean test

test: ./fms_iterable
	./fms_iterable

clean:
	-rm fms_iterable
