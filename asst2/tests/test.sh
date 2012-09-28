#!/bin/bash

set -e
wd=$(pwd)
dir=$(dirname $0)
cd "$dir"
bin="$wd/$1"
fmtstr="%-30s %b\n"

DEF="\033[1;0m"
FAIL="[\033[1;31mFAIL$DEF]"
PASS="[\033[1;32mPASS$DEF]"

if [[ ! -x $bin ]]; then echo "Not executable: $bin"; exit 1; fi

for fname in inputs/*; do
    f=$(basename $fname)

    if "$bin" -g < "$fname" > /dev/null && diff -y --suppress-common-lines -W 200 image.svg "outputs/$f"; then
        printf "$fmtstr" $f "$PASS"
    else
        printf "$fmtstr" $f "$FAIL"
    fi

    rm -f image.svg
done
