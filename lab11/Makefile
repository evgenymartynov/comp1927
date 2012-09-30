BIN  = graph_client
DEPS = GRAPH_adjmatrix.o

CC=gcc
CFLAGS = -Wall -Werror -g

#
# Magic starts here
#

all: ${BIN}
	./${BIN} < ex1.txt
	./${BIN} < ex2.txt
	./${BIN} < ex3-disconnected.txt

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
	rm -f ${BIN} ${BIN}.o ${DEPS} *.gcda *.gcno coverage.info
	rm -rf coverage

# Some book-keeping stuff. Ignore it.
.PHONY: all clean
