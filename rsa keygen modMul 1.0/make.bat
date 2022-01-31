@echo off
rem gcc 5.1.0 (tdm64) win10
gcc rsa.c -Ofast -ansi -Wall -Wextra -pedantic -c
pause
gcc asn1.c -Ofast -ansi -Wall -Wextra -pedantic -c
pause
gcc rsa.o asn1.o main.c -Ofast -ansi -Wall -Wextra -pedantic -o keygen.exe
pause