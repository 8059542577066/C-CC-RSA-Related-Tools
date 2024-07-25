@echo off
rem gcc 5.1.0 (tdm64) win10
gcc sha256.c -O3 -ansi -Wall -Wextra -Wpedantic -pedantic -o sha256.exe
pause