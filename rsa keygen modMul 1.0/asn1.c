#include "asn1.h"
#include <string.h>
#include <stdio.h>


const byte B7 = 1 << 7;
const char *BASE64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


Bytes toBytes(const BigInteger *n)
{
    Bytes r;
    r.size = n->size * 8;
    memcpy(r.bytes, n->uints, r.size);

    while (r.bytes[r.size - 1] == 0 && r.size > 1)
        --r.size;

    return r;
}

Bytes getSizePrefix(int size)
{
    Bytes r;

    if (size <= 0x7F)
        r.size = 1, r.bytes[0] = size;
    else if (size <= 0xFF)
        r.size = 2, r.bytes[0] = 0x81, r.bytes[1] = size;
    else if (size <= 0xFFFF)
        r.size = 3, r.bytes[0] = 0x82, r.bytes[1] = size >> 8,
        r.bytes[2] = size;

    return r;
}

Bytes buildASN1Int(const Bytes *b)
{
    Bytes ts, tc, r;
    int i = 0, j;

    if (b->bytes[b->size - 1] & B7)
        tc.size = b->size + 1, tc.bytes[i++] = 0;
    else
        tc.size = b->size;

    ts = getSizePrefix(tc.size);

    for (j = b->size - 1; j != -1; --j)
        tc.bytes[i++] = b->bytes[j];

    r.size = ts.size + tc.size + 1;
    r.bytes[0] = 0x02, memcpy(&r.bytes[1], ts.bytes, ts.size);
    memcpy(&r.bytes[ts.size + 1], tc.bytes, tc.size);
    return r;
}

Bytes buildPrivatePKCS1(const Bytes *n, const Bytes *e, const Bytes *d,
                        const Bytes *p, const Bytes *q,
                        const Bytes *dP, const Bytes *dQ, const Bytes *qInv)
{
    Bytes ts, r;
    int size = 3 + n->size + e->size + d->size +
               p->size + q->size + dP->size + dQ->size + qInv->size,
        i;
    ts = getSizePrefix(size);
    r.size = ts.size + size + 1, r.bytes[0] = 0x30;
    memcpy(&r.bytes[1], ts.bytes, ts.size), i = ts.size + 1;
    r.bytes[i++] = 0x02, r.bytes[i++] = 0x01, r.bytes[i++] = 0;
    memcpy(&r.bytes[i], n->bytes, n->size), i += n->size;
    memcpy(&r.bytes[i], e->bytes, e->size), i += e->size;
    memcpy(&r.bytes[i], d->bytes, d->size), i += d->size;
    memcpy(&r.bytes[i], p->bytes, p->size), i += p->size;
    memcpy(&r.bytes[i], q->bytes, q->size), i += q->size;
    memcpy(&r.bytes[i], dP->bytes, dP->size), i += dP->size;
    memcpy(&r.bytes[i], dQ->bytes, dQ->size), i += dQ->size;
    memcpy(&r.bytes[i], qInv->bytes, qInv->size);
    return r;
}


void setBase64(const Bytes *b, char *r)
{
    int t = 0, i, j;

    for (i = 0, j = 0; i < b->size - 3; t = 0, i += 3, j += 4)
        t = (int)b->bytes[i] << 16 |
            (int)b->bytes[i + 1] << 8 | (int)b->bytes[i + 2],
        r[j + 3] = BASE64[t % 64], t >>= 6,
        r[j + 2] = BASE64[t % 64], t >>= 6,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];

    if (b->size % 3 == 1)
        t = (int)b->bytes[i] << 16,
        r[j + 3] = '=', r[j + 2] = '=', t >>= 12,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];
    else if (b->size % 3 == 2)
        t = (int)b->bytes[i] << 16 | (int)b->bytes[i + 1] << 8,
        r[j + 3] = '=', t >>= 6,
        r[j + 2] = BASE64[t % 64], t >>= 6,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];
    else
        t = (int)b->bytes[i] << 16 |
            (int)b->bytes[i + 1] << 8 | (int)b->bytes[i + 2],
        r[j + 3] = BASE64[t % 64], t >>= 6,
        r[j + 2] = BASE64[t % 64], t >>= 6,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];

    r[j + 4] = '\0';
}

void printKey(const BigInteger *p, const BigInteger *q,
              const BigInteger *e, const BigInteger *d, const BigInteger *n,
              const BigInteger *dP, const BigInteger *dQ,
              const BigInteger *qInv)
{
    Bytes bp = toBytes(p), bq = toBytes(q),
          be = toBytes(e), bd = toBytes(d), bn = toBytes(n),
          bdP = toBytes(dP), bdQ = toBytes(dQ), bqInv = toBytes(qInv),
          key;
    char buffer[2000];
    int i;
    bp = buildASN1Int(&bp), bq = buildASN1Int(&bq),
    be = buildASN1Int(&be), bd = buildASN1Int(&bd), bn = buildASN1Int(&bn),
    bdP = buildASN1Int(&bdP), bdQ = buildASN1Int(&bdQ),
    bqInv = buildASN1Int(&bqInv);
    key = buildPrivatePKCS1(&bn, &be, &bd, &bp, &bq, &bdP, &bdQ, &bqInv);
    setBase64(&key, buffer);
    printf("-----BEGIN RSA PRIVATE KEY-----");

    for (i = 0; buffer[i] != '\0'; ++i)
        if (i % 64)
            printf("%c", buffer[i]);
        else
            printf("\n%c", buffer[i]);

    printf("\n-----END RSA PRIVATE KEY-----");
}