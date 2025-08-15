all: test

%: %.cpp
	g++ -std=c++20 -Wall -Wextra -fmax-errors=3 -O3 -I../include $< -o $@

test: ../include/callback.hpp ../include/visit-14.hpp ../include/visit-20.hpp
