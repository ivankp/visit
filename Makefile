all: test

%: %.cpp
	g++ -std=c++20 -Wall -Wextra -fmax-errors=3 -O3 -I. $< -o $@

test: callback.hpp visit-14.hpp visit-20.hpp
