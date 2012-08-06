# You can change BIN and DEPS below.

# What my program will be called. Must come from similarly-named .c file.
BIN  = lists
# What my secondary files are. Note that they end in ".o": do NOT use ".c"!!!
DEPS = testLists.o listPredicates.o

CC=gcc
CFLAGS = -Wall -Werror -g

#
# Magic starts here
#

all: ${BIN}
	./${BIN}

# This line means that BIN depends on BIN.o (compiled BIN) and all the DEPS.
# And also on BIN.h.
# Recall that BIN and DEPS are like #defines here.
${BIN}: ${BIN}.o ${DEPS}

# This will remove all compiled files
clean:
	rm ${BIN} ${BIN}.o ${DEPS}

# Some book-keeping stuff. Ignore it.
.PHONY: all clean