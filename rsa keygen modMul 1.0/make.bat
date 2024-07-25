@echo off
rem gcc 5.1.0 (tdm64) win10
gcc rsa.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -c
pause
gcc asn1.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -c
pause
gcc rsa.o asn1.o main.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -o keygen.exe
pause