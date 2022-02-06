@echo off
echo ***** 1. Extract Private Key from PKCS12 (.pfx) File
openssl pkcs12 -in combined.pfx -nodes -nocerts | openssl rsa -out private.key
pause
echo:
echo:
echo ***** 2. Extract Public Certificate from PKCS12 (.pfx) File
openssl pkcs12 -in combined.pfx -out public.crt -nokeys
pause