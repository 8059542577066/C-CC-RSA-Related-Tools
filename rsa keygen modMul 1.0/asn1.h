#ifndef ASN1_H
#define ASN1_H

#include "rsa.h"


typedef uint_fast8_t byte;

typedef struct
{
    int len;
    byte arr[BITS / 2 * 12 / 8];
} Bytes;


void printKey(const BigInt *p, const BigInt *q,
              const BigInt *e, const BigInt *d, const BigInt *n,
              const BigInt *dP, const BigInt *dQ,
              const BigInt *qInv);


#endif