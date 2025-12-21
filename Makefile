# Makefile
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

all: nbody

nbody: main.cpp
	$(CXX) $(CXXFLAGS) -o nbody main.cpp

clean:
	rm -f nbody orbit.dat

run: nbody
	./nbody
