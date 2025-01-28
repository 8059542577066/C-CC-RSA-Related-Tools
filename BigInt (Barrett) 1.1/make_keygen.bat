@echo off
rem gcc 5.1.0 (tdm64) win10
g++ keygen.cc -O3 -std=c++11 -Wall -Wextra -Wpedantic -pedantic -L./ -lbigint -lasn1 -o keygen.exe
pause