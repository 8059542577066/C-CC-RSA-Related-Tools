#include "asn1.h"
#include <string.h>
#include <stdio.h>


const byte B7 = 1 << 7;
const char *BASE64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


Bytes toBytes(const BigInt *n)
{
    Bytes r;
    r.len = n->len * 8;
    memcpy(r.arr, n->arr, r.len);

    while (r.arr[r.len - 1] == 0 && r.len > 1)
        --r.len;

    return r;
}

Bytes getSizePrefix(int len)
{
    Bytes r;

    if (len <= 0x7F)
        r.len = 1, r.arr[0] = len;
    else if (len <= 0xFF)
        r.len = 2, r.arr[0] = 0x81, r.arr[1] = len;
    else if (len <= 0xFFFF)
        r.len = 3, r.arr[0] = 0x82, r.arr[1] = len >> 8,
        r.arr[2] = len;

    return r;
}

Bytes buildASN1Int(const Bytes *b)
{
    Bytes ts, tc, r;
    int i = 0, j;

    if (b->arr[b->len - 1] & B7)
        tc.len = b->len + 1, tc.arr[i++] = 0;
    else
        tc.len = b->len;

    ts = getSizePrefix(tc.len);

    for (j = b->len - 1; j != -1; --j)
        tc.arr[i++] = b->arr[j];

    r.len = ts.len + tc.len + 1;
    r.arr[0] = 0x02, memcpy(&r.arr[1], ts.arr, ts.len);
    memcpy(&r.arr[ts.len + 1], tc.arr, tc.len);
    return r;
}

Bytes buildPrivatePKCS1(const Bytes *n, const Bytes *e, const Bytes *d,
                        const Bytes *p, const Bytes *q,
                        const Bytes *dP, const Bytes *dQ, const Bytes *qInv)
{
    Bytes ts, r;
    int len = 3 + n->len + e->len + d->len +
              p->len + q->len + dP->len + dQ->len + qInv->len,
        i;
    ts = getSizePrefix(len);
    r.len = ts.len + len + 1, r.arr[0] = 0x30;
    memcpy(&r.arr[1], ts.arr, ts.len), i = ts.len + 1;
    r.arr[i++] = 0x02, r.arr[i++] = 0x01, r.arr[i++] = 0;
    memcpy(&r.arr[i], n->arr, n->len), i += n->len;
    memcpy(&r.arr[i], e->arr, e->len), i += e->len;
    memcpy(&r.arr[i], d->arr, d->len), i += d->len;
    memcpy(&r.arr[i], p->arr, p->len), i += p->len;
    memcpy(&r.arr[i], q->arr, q->len), i += q->len;
    memcpy(&r.arr[i], dP->arr, dP->len), i += dP->len;
    memcpy(&r.arr[i], dQ->arr, dQ->len), i += dQ->len;
    memcpy(&r.arr[i], qInv->arr, qInv->len);
    return r;
}


void setBase64(const Bytes *b, char *r)
{
    int t = 0, i, j;

    for (i = 0, j = 0; i < b->len - 3; t = 0, i += 3, j += 4)
        t = (int)b->arr[i] << 16 |
            (int)b->arr[i + 1] << 8 | (int)b->arr[i + 2],
        r[j + 3] = BASE64[t % 64], t >>= 6,
        r[j + 2] = BASE64[t % 64], t >>= 6,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];

    if (b->len % 3 == 1)
        t = (int)b->arr[i] << 16,
        r[j + 3] = '=', r[j + 2] = '=', t >>= 12,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];
    else if (b->len % 3 == 2)
        t = (int)b->arr[i] << 16 | (int)b->arr[i + 1] << 8,
        r[j + 3] = '=', t >>= 6,
        r[j + 2] = BASE64[t % 64], t >>= 6,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];
    else
        t = (int)b->arr[i] << 16 |
            (int)b->arr[i + 1] << 8 | (int)b->arr[i + 2],
        r[j + 3] = BASE64[t % 64], t >>= 6,
        r[j + 2] = BASE64[t % 64], t >>= 6,
        r[j + 1] = BASE64[t % 64], t >>= 6,
        r[j] = BASE64[t];

    r[j + 4] = '\0';
}

void printKey(const BigInt *p, const BigInt *q,
              const BigInt *e, const BigInt *d, const BigInt *n,
              const BigInt *dP, const BigInt *dQ,
              const BigInt *qInv)
{
    Bytes bp = toBytes(p), bq = toBytes(q),
          be = toBytes(e), bd = toBytes(d), bn = toBytes(n),
          bdP = toBytes(dP), bdQ = toBytes(dQ), bqInv = toBytes(qInv),
          key;
    char buffer[BITS];
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