
all: format

format: format.cc
	g++ -Wall -O2 -o format format.cc -lm

clean:
	rm -f format
