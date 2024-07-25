@echo off
rem gcc 5.1.0 (tdm64) win10
gcc pow.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -c
pause
gcc pow.o main.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -o pow0x.exe
pause