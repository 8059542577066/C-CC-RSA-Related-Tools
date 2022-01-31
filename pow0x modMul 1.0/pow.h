#ifndef POW_H
#define POW_H

#include <stdint.h>

#define BITS 2048


typedef uint_fast64_t uint64;

typedef struct
{
    int size;
    uint64 uints[BITS / 64 + 2];
} BigInteger;


int cmp(const BigInteger *x, const BigInteger *y);

BigInteger modPow(const BigInteger *b, BigInteger e,
                  const BigInteger *m);


#endif