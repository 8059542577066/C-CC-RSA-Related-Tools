#!/bin/sh

echo '***** 1) Generate CA private key.'
keygen 2048 $(keygen 2048) > ca_priv.key
read -p "Press Enter to continue..."

echo '***** 2) Generate self-signed CA certificate.'
openssl req -x509 -new -key ca_priv.key -out ca_cert.crt -days 365
read -p "Press Enter to continue..."

echo '***** 3) Generate private key.'
keygen 2048 $(keygen 2048) > priv.key
read -p "Press Enter to continue..."

echo '***** 4) Generate CSR.'
openssl req -key priv.key -new -out req.csr
read -p "Press Enter to continue..."

echo '***** 5) Sign CSR with CA private key.'
openssl x509 -req -in req.csr \
  -CA ca_cert.crt -CAkey ca_priv.key -CAcreateserial \
  -out cert.crt -days 3650 -sha256
read -p "Press Enter to continue..."

echo '***** 6) Append certificate chain.'
cat cert.crt ca_cert.crt > cert_chain.crt
read -p "Press Enter to continue..."
