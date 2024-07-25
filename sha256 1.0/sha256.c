#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef uint_fast8_t byte;
typedef uint_fast32_t uint32;
typedef uint_fast64_t uint64;

const uint32 K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const uint32 H[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


size_t size_byte(size_t bytes)
{
    return ((bytes + 9) / 64 + 1) * 64;
}


uint32 flip_endian_32(uint32 x)
{
    return (x & 0x000000FF) << 8 * 3 |
           (x >> 8 & 0x000000FF) << 8 * 2 |
           (x >> 8 * 2 & 0x000000FF) << 8 |
           x >> 8 * 3;
}

uint32 shr(uint32 x, size_t by)
{
    return x >> by;
}

uint32 ror(uint32 x, size_t by)
{
    return x >> by | x << (32 - by);
}

uint32 sig0(uint32 x)
{
    return ror(x, 7) ^ ror(x, 18) ^ shr(x, 3);
}

uint32 sig1(uint32 x)
{
    return ror(x, 17) ^ ror(x, 19) ^ shr(x, 10);
}

uint32 SIG0(uint32 x)
{
    return ror(x, 2) ^ ror(x, 13) ^ ror(x, 22);
}

uint32 SIG1(uint32 x)
{
    return ror(x, 6) ^ ror(x, 11) ^ ror(x, 25);
}

uint32 cho(uint32 x, uint32 y, uint32 z)
{
    return (x & y) ^ (~x & z);
}

uint32 maj(uint32 x, uint32 y, uint32 z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32 next_W(const uint32 *W)
{
    return sig1(W[-2]) + W[-7] + sig0(W[-15]) + W[-16];
}


void set_size(uint32 *end, uint64 size)
{
    end[-2] = size * 8 >> 32;
    end[-1] = size * 8;
}

void compress(const uint32 *data, uint32 *hash)
{
    uint32 W[64], temp[8];
    size_t i, j;

    for (i = 0; i < 16; ++i)
        W[i] = data[i];

    for (; i < 64; ++i)
        W[i] = next_W(W + i);

    for (j = 0; j < 8; ++j)
        temp[j] = hash[j];

    for (i = 0; i < 64; ++i)
    {
        uint32 T1 = SIG1(temp[4]) +
                    cho(temp[4], temp[5], temp[6]) +
                    temp[7] + K[i] + W[i],
               T2 = SIG0(temp[0]) +
                    maj(temp[0], temp[1], temp[2]);

        for (j = 7; j > 0; --j)
            temp[j] = temp[j - 1];

        temp[0] = T1 + T2;
        temp[4] += T1;
    }

    for (j = 0; j < 8; ++j)
        hash[j] += temp[j];
}

void digest(const char *name)
{
    size_t size, paddedSize, i;
    uint32 *data, hash[8];
    FILE *file = fopen64(name, "rb");

    if (file == NULL)
    {
        printf(" *** ERROR: File does not exist.");
        return;
    }

    fseeko64(file, 0, SEEK_END), size = ftello64(file);
    paddedSize = size_byte(size), rewind(file);
    data = (uint32 *)calloc(paddedSize, 1);
    fread((byte *)data, 1, size, file), fclose(file);
    *((byte *)data + size) = 0x80;

    for (i = 0; i < paddedSize / 4; ++i)
        data[i] = flip_endian_32(data[i]);

    set_size(data + paddedSize / 4, size);

    for (i = 0; i < 8; ++i)
        hash[i] = H[i];

    for (i = 0; i < paddedSize / 64; ++i)
        compress(data + i * 16, hash);

    free(data);

    for (i = 0; i < 8; ++i)
        printf("%08X", hash[i]);
}


int main(int argc, char **argv)
{
    if (argc != 2)
        printf(" *** Usage: sha256 [File Name]");
    else
        digest(argv[1]);

#ifndef _WIN32
    printf("\n");
#endif
    return 0;
}