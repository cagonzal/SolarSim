# Makefile
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

all: nbody

# nbody: main.cpp
nbody: hohmann.cpp
	$(CXX) $(CXXFLAGS) -o nbody hohmann.cpp

clean:
	rm -f nbody orbit.dat out.log

run: nbody
	./nbody
