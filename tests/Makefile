EXE := test-cpp14 test-cpp20

.PHONY: all clean

all: $(EXE)

$(EXE): test-cpp%: test.cpp test.hpp ../include/visit-cpp%.hpp ../include/callback.hpp
	g++ -std=c++$* -Wall -Wextra -fmax-errors=3 -O3 -I../include $< -o $@

clean:
	@rm -fv $(EXE)
