# You can change BIN and DEPS below.

# What my program will be called. Must come from similarly-named .c file.
BIN  = test_allocator
# What my secondary files are. Note that they end in ".o": do NOT use ".c"!!!
DEPS = allocator.o

CC=gcc
CFLAGS = -Wall -Werror -g

#
# Magic starts here
#

all: ${BIN}
	./${BIN}

coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: all
	lcov -o coverage.info -c -d .
	genhtml coverage.info -o coverage

# This line means that BIN depends on BIN.o (compiled BIN) and all the DEPS.
# And also on BIN.h.
# Recall that BIN and DEPS are like #defines here.
${BIN}: ${BIN}.o ${DEPS}

# This will remove all compiled files
clean:
	rm -f ${BIN} ${BIN}.o ${DEPS}
	rm -f coverage.info
	rm -rf coverage

# Some book-keeping stuff. Ignore it.
.PHONY: all clean
