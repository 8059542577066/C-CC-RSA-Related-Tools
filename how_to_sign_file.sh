#!/bin/bash

echo '***** 0) Create dummy file.'
echo `for i in {1..65537}; do echo -n 'c'; done` > file.txt
read -p "Press Enter to continue..."

echo '***** 1) Generate RSA private key.'
keygen 2048 `keygen 2048` > private.key
read -p "Press Enter to continue..."

echo '***** 2) Extract public key from private key.'
openssl rsa -in private.key -pubout -out public.key
read -p "Press Enter to continue..."

echo '***** 3) Sign file.txt with private key.'
openssl dgst -sha256 -sign private.key -hex -out file.txt.sig file.txt
read -p "Press Enter to continue..."

echo '***** 4) Verify signature with public key.'
cut -d' ' -f2 file.txt.sig |\
  xxd -r -p |\
  openssl dgst -sha256 -verify public.key -signature /dev/stdin file.txt
read -p "Press Enter to continue..."
