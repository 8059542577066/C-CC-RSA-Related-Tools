@echo off
rem gcc 5.1.0 (tdm64) win10
gcc pow.c -Ofast -ansi -Wall -Wextra -pedantic -c
pause
gcc pow.o main.c -Ofast -ansi -Wall -Wextra -pedantic -o pow0x.exe
pause