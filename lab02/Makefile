# You can change BIN and DEPS below.

# What my program will be called. Must come from similarly-named .c file.
# I have lists.c
BIN  = lists
# What my secondary files are. Note that they end in ".o": do NOT use ".c"!!!
# I have testLists.c, and I want to compile together it with lists.c
DEPS = testLists.o

CC=gcc
CFLAGS = -Wall -Werror -g   # Change this as you see fit

#
# Magic starts here
#

all: ${BIN}
	./${BIN}

# This line means that BIN depends on BIN.o (compiled BIN) and all the DEPS.
# And also on BIN.h.
# Recall that BIN and DEPS are like #defines here.
${BIN}: ${BIN}.o ${BIN}.h ${DEPS}

# This ensures that whenever you change a header file, corresponding
# things get recompiled.
%.o: %.c %.h

# This will remove all compiled files
clean:
	rm ${BIN} ${BIN}.o ${DEPS}

# Some book-keeping stuff. Ignore it.
.PHONY: all clean