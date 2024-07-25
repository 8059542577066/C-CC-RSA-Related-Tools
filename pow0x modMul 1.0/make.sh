#!/bin/bash
gcc pow.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -c
gcc pow.o main.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -o pow0x
