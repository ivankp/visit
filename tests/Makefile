all: test-cpp14 test-cpp20

test-cpp14 test-cpp20: test-cpp%: test.cpp test.hpp ../include/visit-cpp%.hpp ../include/callback.hpp
	g++ -std=c++$* -Wall -Wextra -fmax-errors=3 -O3 -I../include $< -o $@
