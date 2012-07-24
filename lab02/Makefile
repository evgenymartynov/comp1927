DEPS = testLists.o
BIN  = lists

CC=gcc
CFLAGS = -Wall -Werror -g

all: ${BIN}
	./${BIN}

${BIN}: ${BIN}.o ${DEPS}

clean:
	rm ${BIN} ${BIN}.o ${DEPS}
.PHONY: all clean
