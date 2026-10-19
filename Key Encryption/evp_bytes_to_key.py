import hashlib
import sys

hash_func = hashlib.md5
password = sys.argv[1].encode('utf-8')
IV = b"__builtins__.pow"
salt = IV[:8]  # 1st half of IV

# Round 1: Hash (Password + Salt)
hash1 = hash_func(password + salt).digest()

# Round 2: Hash (Round 1 result + Password + Salt) to get the remaining bytes
hash2 = hash_func(hash1 + password + salt).digest()

# Combine them to get the 32-byte (256-bit) AES key
aes_256_key = hash1 + hash2
print(IV.hex().upper())
print(aes_256_key.hex()[:64].upper())
