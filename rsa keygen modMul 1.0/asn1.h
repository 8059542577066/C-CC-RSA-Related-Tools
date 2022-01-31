#ifndef ASN1_H
#define ASN1_H

#include "rsa.h"

#define SIZE BITS / 2 * 12 / 8


typedef unsigned char byte;

typedef struct
{
    int size;
    byte bytes[SIZE];
} Bytes;


void printKey(const BigInteger *p, const BigInteger *q,
              const BigInteger *e, const BigInteger *d, const BigInteger *n,
              const BigInteger *dP, const BigInteger *dQ,
              const BigInteger *qInv);


#endif