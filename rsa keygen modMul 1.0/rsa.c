#include "rsa.h"
#include <stdlib.h>

#define B_MOD 10
#define N_MOD (1 << (B_MOD + 1))
#define M_MOD ((1 << B_MOD) - 1)

#define B_POW 4
#define N_POW (1 << B_POW)
#define M_POW (N_POW - 1)


typedef __uint128_t uint128;

BigInt Zero()
{
    BigInt r;
    r.len = 1, r.arr[0] = 0;
    return r;
}

BigInt One()
{
    BigInt r;
    r.len = 1, r.arr[0] = 1;
    return r;
}


int cmp(const BigInt *x, const BigInt *y)
{
    int i;

    if (x->len != y->len)
        return x->len > y->len ? 1 : -1;

    for (i = x->len - 1; i > -1; --i)
        if (x->arr[i] != y->arr[i])
            return x->arr[i] > y->arr[i] ? 1 : -1;

    return 0;
}

int index(const BigInt *x,
          const BigInt m_mul[], int i1, int i2)
{
    while (1)
    {
        int i = (i1 + i2) / 2,
            c = cmp(x, &m_mul[i]);

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


void shl(BigInt *x, int by)
{
    const uint64 MASK_L = (uint64)-1 << (64 - by);
    int i = x->len++;
    x->arr[i] = (x->arr[i - 1] & MASK_L) >> (64 - by);

    for (--i; i > 0; --i)
        x->arr[i] = (x->arr[i - 1] & MASK_L) >> (64 - by) |
                    x->arr[i] << by;

    x->arr[0] <<= by;

    if (x->arr[x->len - 1] == 0)
        --x->len;
}

void shr(BigInt *x, int by)
{
    const uint64 MASK_R = (uint64)-1 >> (64 - by);
    int i;

    for (i = 0; i < x->len - 1; ++i)
        x->arr[i] = (x->arr[i + 1] & MASK_R) << (64 - by) |
                    x->arr[i] >> by;

    x->arr[i] >>= by;

    if (x->arr[i] == 0 && x->len > 1)
        --x->len;
}

void add(BigInt *x, const BigInt *y)
{
    int sml, big, i;
    const BigInt *b;
    uint128 t;
    uint64 c = 0;

    if (x->len < y->len)
        sml = x->len, big = y->len, b = y;
    else
        sml = y->len, big = x->len, b = x;

    for (i = 0; i < sml; ++i)
        t = (uint128)x->arr[i] + y->arr[i] + c,
        x->arr[i] = t, c = t >> 64;

    while (i < big)
        t = (uint128)b->arr[i] + c,
        x->arr[i++] = t, c = t >> 64;

    x->len = big;

    if (c > 0)
        x->arr[x->len++] = c;
}

void sub(BigInt *x, const BigInt *y)
{
    uint128 t;
    uint64 c = 0;
    int i;

    for (i = 0; i < y->len; ++i)
        t = (uint128)x->arr[i] - y->arr[i] - c,
        x->arr[i] = t, c = t << 63 >> 127;

    while (i < x->len)
        t = (uint128)x->arr[i] - c,
        x->arr[i++] = t, c = t << 63 >> 127;

    while (x->arr[--i] == 0 && x->len > 1)
        --x->len;
}


BigInt muld(const BigInt *x, uint64 y)
{
    BigInt r;
    uint128 t;
    uint64 c = 0;
    int i;

    if (y == 0)
        return Zero();

    for (i = 0; i < x->len; ++i)
        t = (uint128)x->arr[i] * y + c,
        r.arr[i] = t, c = t >> 64;

    r.len = x->len;

    if (c > 0)
        r.arr[r.len++] = c;

    return r;
}

BigInt modMul(const BigInt *x, BigInt y,
              const BigInt m_mul[])
{
    const BigInt zero = Zero();
    BigInt r = Zero(), t;
    unsigned temp[BITS / B_MOD + 1];
    int i;

    for (i = 0; cmp(&y, &zero) > 0; shr(&y, B_MOD))
        temp[i++] = y.arr[0] & M_MOD;

    for (--i; i > -1; --i)
        t = muld(x, temp[i]), shl(&r, B_MOD), add(&r, &t),
        sub(&r, &m_mul[index(&r, m_mul, 0, N_MOD)]);

    return r;
}

BigInt modPow(const BigInt *b, BigInt e,
              const BigInt *m)
{
    const BigInt zero = Zero();
    BigInt r = One(), t = Zero(),
           *m_mul = malloc(sizeof(BigInt) * N_MOD),
           *b_pow = malloc(sizeof(BigInt) * N_POW);
    unsigned temp[BITS / B_POW + 1];
    int i, j;

    for (i = 0; i < N_MOD; ++i)
        m_mul[i] = t, add(&t, m);

    for (b_pow[0] = One(), i = 1; i < N_POW; ++i)
        b_pow[i] = modMul(&b_pow[i - 1], *b, m_mul);

    for (i = 0; cmp(&e, &zero) > 0; shr(&e, B_POW))
        temp[i++] = e.arr[0] & M_POW;

    for (--i; i > -1; --i)
    {
        for (j = 0; j < B_POW; ++j)
            r = modMul(&r, r, m_mul);

        r = modMul(&r, b_pow[temp[i]], m_mul);
    }

    free(m_mul), free(b_pow);
    return r;
}


BigInt Rand()
{
    BigInt r;
    r.len = 1, r.arr[0] = rand();
    return r;
}

BigInt PubExp()
{
    BigInt r;
    r.len = 1, r.arr[0] = 65537;
    return r;
}


void setProbPrime(BigInt *n)
{
    const BigInt one = One();
    BigInt t = Rand(), n_1 = *n, r;
    uint64 inc = n->arr[0] < (uint64)-0x10000000 ? 2 : -2;
    n_1.arr[0] &= (uint64)-2, n->arr[0] |= 1;
    r = modPow(&t, n_1, n);

    while (cmp(&t, &one) == 0 || cmp(&r, &one) != 0)
        t = Rand(), n_1.arr[0] += inc, n->arr[0] += inc,
        r = modPow(&t, n_1, n);
}

void fixBit(BigInt *n, int bit)
{
    n->len = (bit - 1) / 64 + 1;
    n->arr[n->len - 1] |= (uint64)1 << (bit - 1) % 64;
}

void _add(BigInt *x, const BigInt *y, int d)
{
    const BigInt zero = Zero();
    uint128 t;
    uint64 c = 0;
    int i;

    if (cmp(y, &zero) == 0)
        return;

    for (i = d; i < y->len + d; ++i)
        t = (uint128)(i < x->len ? x->arr[i] : 0) +
            y->arr[i - d] + c,
        x->arr[i] = t, c = t >> 64;

    x->len = y->len + d;

    if (c > 0)
        x->arr[x->len++] = c;
}


BigInt _mul(const BigInt *x, const BigInt *y)
{
    BigInt r = Zero(), t;
    int i;

    for (r.len = x->len + y->len, i = 0; i < y->len; ++i)
        t = muld(x, y->arr[i]), _add(&r, &t, i);

    if (r.arr[r.len - 1] == 0)
        --r.len;

    return r;
}

BigInt _mod(BigInt x, const BigInt *y)
{
    BigInt t = *y;

    while (cmp(&x, &t) >= 0)
        shl(&t, 1);

    for (shr(&t, 1); cmp(&x, y) >= 0; shr(&t, 1))
        if (cmp(&x, &t) >= 0)
            sub(&x, &t);

    return x;
}

BigInt _div(BigInt x, const BigInt *y)
{
    BigInt r = Zero(), t = *y, p = One();

    while (cmp(&x, &t) >= 0)
        shl(&t, 1), shl(&p, 1);

    shr(&t, 1), shr(&p, 1);

    while (cmp(&x, y) >= 0)
    {
        if (cmp(&x, &t) >= 0)
            sub(&x, &t), add(&r, &p);

        shr(&t, 1), shr(&p, 1);
    }

    return r;
}

BigInt modInv(const BigInt *b, const BigInt *m)
{
    const BigInt zero = Zero();
    BigInt t1 = *m, t2;
    uint64 k = 1;
    ++t1.arr[0], t2 = _mod(t1, b);

    while (cmp(&t2, &zero) > 0)
        t1 = muld(m, ++k), ++t1.arr[0], t2 = _mod(t1, b);

    return _div(t1, b);
}

BigInt random(int bits)
{
    BigInt r = Zero(), t, p = One(), mask = One();
    int i;

    for (i = 0; i < bits; ++i)
        shl(&mask, 1);

    while (cmp(&r, &mask) < 0)
        t = muld(&p, rand()), add(&r, &t),
        p = muld(&p, (uint64)RAND_MAX + 1);

    return _mod(r, &mask);
}


void setKey(int bits, BigInt *p, BigInt *q,
            BigInt *e, BigInt *d, BigInt *n,
            BigInt *dP, BigInt *dQ, BigInt *qInv)
{
    const BigInt one = One();
    BigInt p_1, q_1, t1, t2;
    *q = random(bits), fixBit(q, bits), *q = _div(*q, p);
    setProbPrime(p), setProbPrime(q), *e = PubExp();
    p_1 = *p, q_1 = *q, --p_1.arr[0], --q_1.arr[0];
    t1 = _mul(&p_1, &q_1), t2 = _mod(t1, e);

    if (cmp(&t2, &one) < 0)
        setKey(bits, p, q, e, d, n, dP, dQ, qInv);
    else
    {
        BigInt m = Rand(), c, _m;
        *d = modInv(e, &t1), *n = _mul(p, q);
        c = modPow(&m, *e, n), _m = modPow(&c, *d, n);

        if (cmp(&m, &one) > 0 && cmp(&m, &_m) == 0)
        {
            if (cmp(p, q) < 0)
                t1 = *p, *p = *q, *q = t1,
                t1 = p_1, p_1 = q_1, q_1 = t1;

            *dP = _mod(*d, &p_1), *dQ = _mod(*d, &q_1);
            sub(&p_1, &one), *qInv = modPow(q, p_1, p);
        }
        else
            setKey(bits, p, q, e, d, n, dP, dQ, qInv);
    }
}