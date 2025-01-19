@echo off
rem gcc 5.1.0 (tdm64) win10
g++ main.cc -O3 -std=c++11 -Wall -Wextra -Wpedantic -pedantic -L./ -lbigint -o pow0x.exe
pause