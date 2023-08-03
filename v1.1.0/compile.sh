#/bin/bash

gcc src/*.c src/memory/bintree.c src/memory/ptrstack.c src/memory/stack.c src/memory/memutils.c src/main/main.c -lm -g -o $1
