DEPS = testLists.o
BIN  = lists

CC=gcc
CFLAGS = -O -Wall -Werror -g

all: ${BIN}
	./${BIN}

${BIN}: ${BIN}.o ${DEPS}

clean:
	rm ${BIN} ${DEPS}
.PHONY: all clean
