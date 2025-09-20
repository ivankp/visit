.PHONY: all clean

SRC := $(wildcard test-*.cpp)
EXE := $(SRC:.cpp=)

all: $(EXE)
	@for x in $^; do echo $$x; ./$$x; done

$(EXE): %: %.cpp test.hpp
	g++ -std=c++17 -Wall -Wextra -Werror -pedantic -O3 -I. -I../include $< -o $@

test-inheritance: visit-inheritance.hpp

clean:
	@rm -fv $(EXE)
