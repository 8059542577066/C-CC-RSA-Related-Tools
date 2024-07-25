#ifndef RSA_H
#define RSA_H

#include <stdint.h>

#ifndef BITS
#define BITS 2048
#endif


typedef uint_fast64_t uint64;

typedef struct
{
    int len;
    uint64 arr[BITS / 64 + 2];
} BigInt;


void setKey(int bits, BigInt *p, BigInt *q,
            BigInt *e, BigInt *d, BigInt *n,
            BigInt *dP, BigInt *dQ, BigInt *qInv);


#endif