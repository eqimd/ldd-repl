#!/bin/bash

for filename in $(find /usr/bin/ -maxdepth 1 -type f -executable); do
    echo "Current file: $filename"
    ../ldd-repl $filename > repl.out
    ldd $filename > orig.out
    python3 compare.py orig.out repl.out
    if [[ $? == 1 ]]; then
        exit 1
    fi
done

echo "Success!"

rm orig.out repl.out
