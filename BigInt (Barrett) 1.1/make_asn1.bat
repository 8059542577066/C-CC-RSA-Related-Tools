@echo off
rem gcc 5.1.0 (tdm64) win10
g++ asn1.cc -O3 -std=c++11 -Wall -Wextra -Wpedantic -pedantic -L./ -lbigint -DBUILD_LIB -shared -o asn1.dll
pause