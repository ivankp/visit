EXAMPLES := $(wildcard ../examples/*.hpp)
TESTS := $(EXAMPLES:../examples/%.hpp=tests-%)

.PHONY: all clean

all: $(TESTS) failure/test.sh
	@for tests in $^; do printf "\n$$tests\n"; ./$$tests; done

$(TESTS): tests-%: tests.cpp test.hpp ../examples/%.hpp ../include/visit.hpp ../include/callable.hpp
	g++ -std=c++17 -Wall -Wextra -Werror -pedantic -O3 -DEXAMPLE=$* -I. -I../include $< -o $@

clean:
	@rm -fv $(TESTS)
