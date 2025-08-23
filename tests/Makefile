EXE := test-cpp14 test-cpp20 test-direct

.PHONY: all clean

all: $(EXE)

test-cpp14 test-cpp20: test-cpp%: test.cpp test.hpp ../include/visit-cpp%.hpp ../include/callback.hpp
	g++ -std=c++$* -Wall -Wextra -fmax-errors=3 -O3 -I../include $< -o $@

test-direct: test.cpp test.hpp ../include/callback.hpp
	g++ -std=c++20 -Wall -Wextra -fmax-errors=3 -O3 -DTEST_DIRECT -I../include $< -o $@

clean:
	@rm -fv $(EXE)
