#!/bin/bash

filename="hello_world"
echo "Current file: $filename"
../ldd-repl $filename > repl.out
ldd $filename > orig.out
python3 compare.py orig.out repl.out
if [[ $? == 1 ]]; then
    exit 1
fi

echo "Success!"

rm orig.out repl.out
