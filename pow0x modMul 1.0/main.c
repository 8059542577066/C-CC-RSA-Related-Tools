#include "pow.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define MAX_LEN BITS / 4


int isxstr(const char *str)
{
    while (*str != '\0')
        if (!isxdigit(*str++))
            return 0;

    return 1;
}

int isall0(const char *str)
{
    while (*str != '\0')
        if (*str++ != '0')
            return 0;

    return 1;
}


void scan(const char *str, BigInt *r)
{
    char buffer[MAX_LEN + 1];
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

void print(const BigInt *n)
{
    int i = n->len - 1;

    if (n->arr[i] >> 32 > 0)
    {
        printf("%X", (unsigned)(n->arr[i] >> 32));
        printf("%08X", (unsigned)n->arr[i]);
    }
    else
        printf("%X", (unsigned)n->arr[i]);

    for (--i; i > -1; --i)
    {
        printf("%08X", (unsigned)(n->arr[i] >> 32));
        printf("%08X", (unsigned)n->arr[i]);
    }
}


int main(int argc, char **argv)
{
    if (argc != 4)
        printf(" *** Usage: pow0x [b] [e] [m]");
    else if (strlen(argv[1]) > MAX_LEN)
        printf(" *** ERROR: [b] too long");
    else if (strlen(argv[2]) > MAX_LEN)
        printf(" *** ERROR: [e] too long");
    else if (strlen(argv[3]) > MAX_LEN)
        printf(" *** ERROR: [m] too long");
    else if (!isxstr(argv[1]))
        printf(" *** ERROR: [b] not hex");
    else if (!isxstr(argv[2]))
        printf(" *** ERROR: [e] not hex");
    else if (!isxstr(argv[3]))
        printf(" *** ERROR: [m] not hex");
    else if (isall0(argv[3]))
        printf(" *** ERROR: [m] is 0");
    else
    {
        BigInt b, e, m, r;
        scan(argv[1], &b);
        scan(argv[2], &e);
        scan(argv[3], &m);

        if (cmp(&b, &m) >= 0)
            printf(" *** ERROR: [b] >= [m]");
        else
            r = modPow(&b, e, &m),
            print(&r);
    }

#ifndef _WIN32
    printf("\n");
#endif
    return 0;
}