
CXX=c++

all: format test

format: format.cc
	$(CXX) -Wall -O2 -o format format.cc -lm

test: test.cc
	$(CXX) -Wall -g -O0 -o test test.cc -pthread

clean:
	rm -f format test
