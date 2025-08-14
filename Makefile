all: test

%: %.cpp
	g++ -std=c++20 -Wall -Wextra -O3 -fmax-errors=3 -I. $< -o $@
