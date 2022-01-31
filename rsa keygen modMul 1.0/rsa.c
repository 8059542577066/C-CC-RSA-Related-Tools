#include "rsa.h"
#include <stdlib.h>

#define B_MOD 12
#define N_MOD (1 << (B_MOD + 1))
#define M_MOD ((1 << B_MOD) - 1)

#define B_POW 5
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


BigInteger Rand()
{
    BigInteger r;
    r.size = 1, r.uints[0] = rand();
    return r;
}

BigInteger PubExp()
{
    BigInteger r;
    r.size = 1, r.uints[0] = 65537;
    return r;
}


int eq1(const BigInteger *x)
{
    return x->size == 1 && x->uints[0] == 1;
}


void setProbPrime(BigInteger *n)
{
    BigInteger t = Rand(), n_1 = *n, r;
    uint64 inc = n->uints[0] < (uint64)-0x10000000 ? 2 : -2;
    n_1.uints[0] &= (uint64)-2, n->uints[0] |= 1;
    r = modPow(&t, n_1, n);

    while (eq1(&t) || !eq1(&r))
        t = Rand(), n_1.uints[0] += inc, n->uints[0] += inc,
        r = modPow(&t, n_1, n);
}

void fixBit(BigInteger *n, int bit)
{
    n->size = (bit - 1) / 64 + 1;
    n->uints[n->size - 1] |= (uint64)1 << (bit - 1) % 64;
}

void _addi(BigInteger *x, const BigInteger *y, int d)
{
    uint128 t;
    uint64 c = 0;
    int i;

    if (eq0(y))
        return;

    for (i = d; i < y->size + d; ++i)
        t = (uint128)(i < x->size ? x->uints[i] : 0) +
            y->uints[i - d] + c,
        x->uints[i] = t, c = t >> 64;

    x->size = y->size + d;

    if (c > 0)
        x->uints[x->size++] = c;
}


BigInteger _mul(const BigInteger *x, const BigInteger *y)
{
    BigInteger r = Zero(), t;
    int i;

    for (r.size = x->size + y->size, i = 0; i < y->size; ++i)
        t = muld(x, y->uints[i]), _addi(&r, &t, i);

    if (r.uints[r.size - 1] == 0)
        --r.size;

    return r;
}

BigInteger _mod(BigInteger x, const BigInteger *y)
{
    BigInteger t = *y;

    while (cmp(&x, &t) >= 0)
        shl(&t, 1);

    for (shr(&t, 1); cmp(&x, y) >= 0; shr(&t, 1))
        if (cmp(&x, &t) >= 0)
            subi(&x, &t);

    return x;
}

BigInteger _div(BigInteger x, const BigInteger *y)
{
    BigInteger r = Zero(), t = *y, p = One();

    while (cmp(&x, &t) >= 0)
        shl(&t, 1), shl(&p, 1);

    shr(&t, 1), shr(&p, 1);

    while (cmp(&x, y) >= 0)
    {
        if (cmp(&x, &t) >= 0)
            subi(&x, &t), addi(&r, &p);

        shr(&t, 1), shr(&p, 1);
    }

    return r;
}

BigInteger modInv(const BigInteger *b, const BigInteger *m)
{
    BigInteger t1 = *m, t2;
    uint64 k = 1;
    ++t1.uints[0], t2 = _mod(t1, b);

    while (!eq0(&t2))
        t1 = muld(m, ++k), ++t1.uints[0], t2 = _mod(t1, b);

    return _div(t1, b);
}

BigInteger random(int bits)
{
    BigInteger r = Zero(), t, p = One(), mask = One();
    int i;

    for (i = 0; i < bits; ++i)
        shl(&mask, 1);

    while (cmp(&r, &mask) < 0)
        t = muld(&p, rand()), addi(&r, &t),
        p = muld(&p, (uint64)RAND_MAX + 1);

    return _mod(r, &mask);
}


void setKey(int bits, BigInteger *p, BigInteger *q,
            BigInteger *e, BigInteger *d, BigInteger *n,
            BigInteger *dP, BigInteger *dQ, BigInteger *qInv)
{
    BigInteger p_1, q_1, t1, t2;
    *q = random(bits), fixBit(q, bits), *q = _div(*q, p);
    setProbPrime(p), setProbPrime(q), *e = PubExp();
    p_1 = *p, q_1 = *q, --p_1.uints[0], --q_1.uints[0];
    t1 = _mul(&p_1, &q_1), t2 = _mod(t1, e);

    if (eq0(&t2))
        setKey(bits, p, q, e, d, n, dP, dQ, qInv);
    else
    {
        BigInteger m = Rand(), c, _m;
        *d = modInv(e, &t1), *n = _mul(p, q);
        c = modPow(&m, *e, n), _m = modPow(&c, *d, n);

        if (!eq0(&m) && !eq1(&m) && cmp(&m, &_m) == 0)
        {
            BigInteger one = One();

            if (cmp(p, q) < 0)
                t1 = *p, *p = *q, *q = t1,
                t1 = p_1, p_1 = q_1, q_1 = t1;

            *dP = _mod(*d, &p_1), *dQ = _mod(*d, &q_1);
            subi(&p_1, &one), *qInv = modPow(q, p_1, p);
        }
        else
            setKey(bits, p, q, e, d, n, dP, dQ, qInv);
    }
}