#!/bin/bash
gcc rsa.c -Ofast -ansi -Wall -Wextra -pedantic -c
gcc asn1.c -Ofast -ansi -Wall -Wextra -pedantic -c
gcc rsa.o asn1.o main.c -Ofast -ansi -Wall -Wextra -pedantic -o keygen
