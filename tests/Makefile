.PHONY: all clean

EXAMPLES := $(wildcard ../examples/*.hpp)
TESTS := $(EXAMPLES:../examples/%.hpp=tests-%)

all: $(TESTS)
	@for tests in $^; do echo $$tests; ./$$tests; done

$(TESTS): tests-%: tests.cpp test.hpp ../examples/%.hpp
	g++ -std=c++17 -Wall -Wextra -Werror -pedantic -O3 -DEXAMPLE=$* -I. -I../include $< -o $@

clean:
	@rm -fv $(TESTS)
