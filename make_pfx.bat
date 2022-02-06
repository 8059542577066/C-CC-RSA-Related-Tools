@echo off
echo ***** 0. Encrypt Private Key
openssl rsa -in private.key -out private.key -aes128
pause
echo:
echo:
echo ***** 1. Create Public Certificate
openssl req -x509 -key private.key -out public.crt -sha256 -set_serial 0 -days 365000
pause
echo:
echo:
echo ***** 2. Create PKCS12 (.pfx) File
openssl pkcs12 -export -in public.crt -inkey private.key -out combined.pfx
pause