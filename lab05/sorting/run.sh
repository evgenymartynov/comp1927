#!/bin/bash

function runrand() {
    n="$1"
    f="$(mktemp)"

    echo "$n" | ./gen > $f 2>/dev/null
    time ./sort < $f > /dev/null 2>/dev/null

    rm $f
}

function runordered() {
    n="$1"
    f="$(mktemp)"

    echo $n > $f
    echo "$n" | ./gen 2>/dev/null | tail -n +2 | sort -n >> $f
    time ./sort < $f > /dev/null 2>/dev/null

    rm $f
}

function runrev() {
    n="$1"
    f="$(mktemp)"

    echo $n > $f
    echo "$n" | ./gen 2>/dev/null | tail -n +2 | sort -n | tac >> $f
    time ./sort < $f > /dev/null 2>/dev/null

    rm $f
}

# n=10000
# echo "$n random"
# for i in {0..10}; do
#     runrand $n
# done
# echo
# 
# n=100000
# echo "$n random"
# for i in {0..3}; do
#     runrand $n &
#     runrand $n &
#     runrand $n
# done

# n=10000
# echo "$n ordered"
# for i in {0..10}; do
#     runordered $n
# done
# echo
# 
# n=100000
# echo "$n ordered"
# for i in {0..3}; do
#     runordered $n &
#     runordered $n &
#     runordered $n
# done


n=10000
echo "$n rev"
for i in {0..10}; do
    runrev $n
done
echo

n=100000
echo "$n rev"
for i in {0..3}; do
    runrev $n &
    runrev $n &
    runrev $n
done
