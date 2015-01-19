CXXFLAGS ?= -Wall -Wextra -g -std=gnu++11
LDLIBS ?= -lmuparser -lreadline

mucalc: mucalc.cpp

clean:
	rm -f mucalc
