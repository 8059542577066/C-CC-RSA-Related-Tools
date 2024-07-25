#include "rsa.h"
#include "asn1.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


int isxstr(const char *str)
{
    while (*str != '\0')
        if (!isxdigit(*str++))
            return 0;

    return 1;
}

int trimlen(const char *str)
{
    while (*str == '0')
        ++str;

    return strlen(str);
}


void scan(const char *str, BigInt *r)
{
    char buffer[300];
    int len = strlen(str), i = 0, j;
    unsigned *ptr;

    if (len % 16 != 0)
        while (i < 16 - len % 16)
            buffer[i++] = '0';

    buffer[i] = '\0', strcat(buffer, str);
    r->len = strlen(buffer) / 16;

    for (i = 0, j = r->len - 1; j > -1; i += 16, --j)
        ptr = (unsigned *)&r->arr[j],
        sscanf(buffer + i, "%08X", ptr + 1),
        sscanf(buffer + i + 8, "%08X", ptr);

    while (r->arr[r->len - 1] == 0 && r->len > 1)
        --r->len;
}

void printerr(const char *msg)
{
    fprintf(stderr, " *** ERROR: %s", msg);
#ifndef _WIN32
    fprintf(stderr, "\n");
#endif
}

void printkey(int bits, BigInt *prime)
{
    BigInt q, e, d, n, dP, dQ, qInv;
    time_t time1 = clock(), time2;
    setKey(bits, prime, &q, &e, &d, &n, &dP, &dQ, &qInv);
    time2 = clock();
    fprintf(stderr, " ### Time: %.3f secs\n",
            (double)(time2 - time1) / CLOCKS_PER_SEC);
    printKey(prime, &q, &e, &d, &n, &dP, &dQ, &qInv);
#ifndef _WIN32
    printf("\n");
#endif
}


int main(int argc, char **argv)
{
    if (argc != 3)
        printerr("keygen [key len] [hex prime]");
    else if (!isxstr(argv[2]))
        printerr("[prime] not hex");
    else
    {
        int bits = atoi(argv[1]),
            lower = bits / 8 - 16, upper = bits / 8 + 16,
            len = trimlen(argv[2]);

        if (bits < BITS / 4 || bits > BITS)
            printerr("Invalid RSA key len");
        else if (len < lower || len > upper)
            printerr("Invalid prime length");
        else
        {
            BigInt prime;
            scan(argv[2], &prime), srand(time(0));
            printkey(bits, &prime);
        }
    }

    return 0;
}