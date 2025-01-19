@echo off
rem gcc 5.1.0 (tdm64) win10
g++ bigint.cc -O3 -std=c++11 -Wall -Wextra -Wpedantic -pedantic -DBUILD_LIB -shared -o bigint.dll
pause