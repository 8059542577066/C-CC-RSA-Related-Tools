#!/bin/bash
gcc rsa.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -c
gcc asn1.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -c
gcc rsa.o asn1.o main.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -o keygen
