#!/bin/bash

echo '-----BEGIN RSA PRIVATE KEY-----'
echo 'Proc-Type: 4,ENCRYPTED'
echo -n 'DEK-Info: AES-256-CBC,'
echo $(python3 evp_bytes_to_key.py $1 | head -n 1)
echo

cat RSA\ PRIVATE\ KEY.txt \
    | head -n -1 | tail -n +2 | base64 -d -i \
    | openssl enc -aes256 -e \
    -iv $(python3 evp_bytes_to_key.py $1 | head -n 1) \
    -K $(python3 evp_bytes_to_key.py $1 | tail -n 1) \
    | base64 -w 64

echo '-----END RSA PRIVATE KEY-----'
