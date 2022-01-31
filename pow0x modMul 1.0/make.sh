#!/bin/bash
gcc pow.c -Ofast -ansi -Wall -Wextra -pedantic -c
gcc pow.o main.c -Ofast -ansi -Wall -Wextra -pedantic -o pow0x
