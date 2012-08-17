#!/bin/bash

# Terminate on errors.
set -e

# When you press ^C, this will terminate all running instances of isort.
# Otherwise you might end up with like 20 sorters running in parallel.
# Not pretty.
trap "rm -f times; killall ./isort" INT

OUTPUT_TABLE="times"

function compile() {
    gcc -Wall -Werror -O0    isort.c   -o isort
    gcc -Wall -Werror -O0    genrandom.c   -o genrandom
}

# Magic. Black, black magic.
function run_test() {
    (time ./isort < $1 > /dev/null 2>/dev/null) 2>&1    \
        | grep -E '^real|^sys'                          \
        | tr -d '\n real sys'                           \
        | sed -r 's#\t#/#2'                             \
        | xargs echo >> $OUTPUT_TABLE
}

# Attempt to parallelise the tests to save time.
function run_batch() {
    echo "Running n=$n with $1"
    echo "$n $1" >> $OUTPUT_TABLE

    for i in {1..5}; do
        for j in {1..2}; do
            ($1 $n; echo -n .) &
        done
        wait
    done

    echo
}

function Random() {
    n="$1"
    f="$(mktemp)"

    echo "$n" | ./genrandom > $f 2>/dev/null
    run_test $f

    rm $f
}

function Ordered() {
    n="$1"
    f="$(mktemp)"

    echo $n > $f
    echo "$n" | ./genrandom 2>/dev/null | tail -n +2 | sort -n >> $f
    run_test $f

    rm $f
}

function Reversed() {
    n="$1"
    f="$(mktemp)"

    echo $n > $f
    echo "$n" | ./genrandom 2>/dev/null | tail -n +2 | sort -n | tac >> $f
    run_test $f

    rm $f
}

function present_table() {
    python - << EOF
L = map(str.strip, open("$OUTPUT_TABLE", "r").readlines())
columns = []
for l in L:
    if '/' not in l:
        columns.append([])
    columns[-1].append(l)

numrows = len(columns[0])
numcols = len(columns)
widths = [max(16, len(columns[i][0])) for i in xrange(numcols)]

for y in xrange(numrows):
    print '| %2.1d  ' % y if y else '|= Run',
    for x in xrange(numcols):
        print ('|%s %%%ds' % (' ' if y else '=', widths[x])) % columns[x][y],
    print
EOF
}

#
# This bit runs the tests:
#

compile
rm -f $OUTPUT_TABLE

for t in Random Ordered Reversed; do
    n=10000
    run_batch $t
    n=100000
    run_batch $t
done

present_table
rm -f $OUTPUT_TABLE
