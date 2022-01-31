#ifndef RSA_H
#define RSA_H

#include <stdint.h>

#define BITS 2048


typedef uint_fast64_t uint64;

typedef struct
{
    int size;
    uint64 uints[BITS / 64 + 2];
} BigInteger;


void setKey(int bits, BigInteger *p, BigInteger *q,
            BigInteger *e, BigInteger *d, BigInteger *n,
            BigInteger *dP, BigInteger *dQ, BigInteger *qInv);


#endif