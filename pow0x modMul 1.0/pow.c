#include "pow.h"
#include <stdlib.h>

#define B_MOD 14
#define N_MOD (1 << (B_MOD + 1))
#define M_MOD ((1 << B_MOD) - 1)

#define B_POW 6
#define N_POW (1 << B_POW)
#define M_POW (N_POW - 1)


typedef __uint128_t uint128;

BigInteger Zero()
{
    BigInteger r;
    r.size = 1, r.uints[0] = 0;
    return r;
}

BigInteger One()
{
    BigInteger r;
    r.size = 1, r.uints[0] = 1;
    return r;
}


int eq0(const BigInteger *x)
{
    return x->size == 1 && x->uints[0] == 0;
}

int cmp(const BigInteger *x, const BigInteger *y)
{
    int i;

    if (x->size != y->size)
        return x->size > y->size ? 1 : -1;

    for (i = x->size - 1; i > -1; --i)
        if (x->uints[i] != y->uints[i])
            return x->uints[i] > y->uints[i] ? 1 : -1;

    return 0;
}

int index(const BigInteger *x,
          const BigInteger arr[], int i1, int i2)
{
    while (1)
    {
        int i = (i1 + i2) / 2,
            c = cmp(x, &arr[i]);

        if (c == 0)
            return i;
        else if (i1 + 1 == i2)
            return i1;
        else if (c < 0)
            i2 = i;
        else
            i1 = i;
    }
}


void borrow(uint64 *uints)
{
    while (*uints == 0)
        --*uints++;

    --*uints;
}

void reduce(BigInteger *x)
{
    while (x->uints[x->size - 1] == 0 && x->size > 1)
        --x->size;
}

void shl(BigInteger *x, int by)
{
    const uint64 MASK_L = (uint64)-1 << (64 - by);
    int i = x->size++;
    x->uints[i] = (x->uints[i - 1] & MASK_L) >> (64 - by);

    for (--i; i > 0; --i)
        x->uints[i] = (x->uints[i - 1] & MASK_L) >> (64 - by) |
                      x->uints[i] << by;

    x->uints[0] <<= by;

    if (x->uints[x->size - 1] == 0)
        --x->size;
}

void shr(BigInteger *x, int by)
{
    const uint64 MASK_R = (uint64)-1 >> (64 - by);
    int i;

    for (i = 0; i < x->size - 1; ++i)
        x->uints[i] = (x->uints[i + 1] & MASK_R) << (64 - by) |
                      x->uints[i] >> by;

    x->uints[i] >>= by, reduce(x);
}

void addi(BigInteger *x, const BigInteger *y)
{
    int sml, big, i;
    const BigInteger *b;
    uint128 t;
    uint64 c = 0;

    if (x->size < y->size)
        sml = x->size, big = y->size, b = y;
    else
        sml = y->size, big = x->size, b = x;

    for (i = 0; i < sml; ++i)
        t = (uint128)x->uints[i] + y->uints[i] + c,
        x->uints[i] = t, c = t >> 64;

    while (i < big)
        t = (uint128)b->uints[i] + c,
        x->uints[i++] = t, c = t >> 64;

    x->size = big;

    if (c > 0)
        x->uints[x->size++] = c;
}

void subi(BigInteger *x, const BigInteger *y)
{
    int i;

    for (i = 0; i < y->size; ++i)
    {
        if (x->uints[i] < y->uints[i])
            borrow(x->uints + i + 1);

        x->uints[i] -= y->uints[i];
    }

    reduce(x);
}


BigInteger muld(const BigInteger *x, uint64 y)
{
    BigInteger r;
    uint128 t;
    uint64 c = 0;
    int i;

    if (y == 0)
        return Zero();

    for (i = 0; i < x->size; ++i)
        t = (uint128)x->uints[i] * y + c,
        r.uints[i] = t, c = t >> 64;

    r.size = x->size;

    if (c > 0)
        r.uints[r.size++] = c;

    return r;
}

BigInteger modMul(const BigInteger *x, BigInteger y,
                  const BigInteger mod[])
{
    BigInteger r = Zero(), t;
    unsigned temp[BITS / B_MOD + 1];
    int i;

    for (i = 0; !eq0(&y); shr(&y, B_MOD))
        temp[i++] = y.uints[0] & M_MOD;

    for (--i; i > -1; --i)
        t = muld(x, temp[i]), shl(&r, B_MOD), addi(&r, &t),
        subi(&r, &mod[index(&r, mod, 0, N_MOD)]);

    return r;
}

BigInteger modPow(const BigInteger *b, BigInteger e,
                  const BigInteger *m)
{
    BigInteger r = One(), t = Zero(),
               *mod = malloc(sizeof(BigInteger) * N_MOD),
               *pow = malloc(sizeof(BigInteger) * N_POW);
    unsigned temp[BITS / B_POW + 1];
    int i, j;

    for (i = 0; i < N_MOD; ++i)
        mod[i] = t, addi(&t, m);

    for (pow[0] = One(), i = 1; i < N_POW; ++i)
        pow[i] = modMul(&pow[i - 1], *b, mod);

    for (i = 0; !eq0(&e); shr(&e, B_POW))
        temp[i++] = e.uints[0] & M_POW;

    for (--i; i > -1; --i)
    {
        for (j = 0; j < B_POW; ++j)
            r = modMul(&r, r, mod);

        r = modMul(&r, pow[temp[i]], mod);
    }

    free(mod), free(pow);
    return r;
}