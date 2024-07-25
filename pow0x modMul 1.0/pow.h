#ifndef POW_H
#define POW_H

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


int cmp(const BigInt *x, const BigInt *y);

BigInt modPow(const BigInt *b, BigInt e,
              const BigInt *m);


#endif