#include "bigint.hh"
#include "asn1.hh"
#include <cstdlib>
#include <ctime>
#include <cstdio>


bool prime(const BigInt &n, int rep)
{
    BigInt a, one;
    one = 1;

    for (int i = 0; i < rep; ++i)
    {
        a = std::rand();

        if (BigInt::pow(a, n - one, n) != one)
            return false;
    }

    return true;
}

void next_prime(BigInt &n)
{
    BigInt zero, one, two;
    one = 1, two = 2;

    if (n % two == zero)
        n += one;

    while (!prime(n, 2))
        n += two;
}

BigInt rand(int bits)
{
    BigInt temp, power, stride, res;
    power = 1, stride = RAND_MAX + 1;

    while (res.bits() <= bits)
        temp = std::rand(), res += temp * power,
        power *= stride;

    power = 1, power <<= bits;
    return res % power;
}

BigInt rand_msb(int bits)
{
    BigInt res = rand(bits), power;
    power = 1, power <<= bits - 1;
    return res < power ? res + power : res;
}

double set_rsa_key(int bits,
                   BigInt &n, BigInt &e, BigInt &d,
                   BigInt &p, BigInt &q,
                   BigInt &dp, BigInt &dq, BigInt &qinv)
{
    std::srand(std::time(0));
    auto time1 = std::clock();
    n = rand_msb(bits), q = n / p;
    next_prime(p), next_prime(q);
    BigInt one, m;
    e = 65537, n = p * q, one = 1, m = rand(bits / 2);
    d = BigInt::inv(e, (p - one) * (q - one));

    if (BigInt::pow(BigInt::pow(m, e, n), d, n) == m)
    {
        if (p < q)
        {
            BigInt temp = p;
            p = q, q = temp;
        }

        BigInt two;
        dp = d % (p - one), dq = d % (q - one), two = 2;
        qinv = BigInt::pow(q, p - two, p);
    }
    else
        set_rsa_key(bits, n, e, d, p, q, dp, dq, qinv);

    auto time2 = std::clock();
    return (double)(time2 - time1) / CLOCKS_PER_SEC;
}


void print_time(double time)
{
    std::fprintf(stderr, " ### Time: %.3f secs\n", time);
}

void print_err(const char *msg)
{
    std::fprintf(stderr, " *** ERROR: %s", msg);
#ifndef _WIN32
    std::fprintf(stderr, "\n");
#endif
}


int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3)
    {
        print_err("keygen [key len] [hex prime]\n"
                  "        or  keygen [key len]");
        return -1;
    }

    int bits = std::atoi(argv[1]);

    if (bits < 512)
    {
        print_err("[key len] too small");
        return -1;
    }
    else if (argc == 2)
    {
        std::srand(std::time(0));
        char *str = rand(bits / 2).c_str();
        std::printf(str);
        delete[] str;
        return 0;
    }

    BigInt p;

    try
    {
        p.scan(argv[2]);
    }
    catch (int code)
    {
        print_err("[prime] not hex");
        return -1;
    }

    int lower = bits / 2 * 15 / 16,
        upper = bits - lower,
        p_bits = p.bits();

    if (p_bits < lower || p_bits > upper)
    {
        print_err("Invalid [hex prime] length");
        return -1;
    }

    BigInt ver, n, e, d, q, dp, dq, qinv;
    print_time(set_rsa_key(bits, n, e, d,
                           p, q, dp, dq, qinv));
    Bytes bytes = Bytes(ver).pack(0x02);
    bytes += Bytes(n).pack(0x02) +
             Bytes(e).pack(0x02) +
             Bytes(d).pack(0x02);
    bytes += Bytes(p).pack(0x02) +
             Bytes(q).pack(0x02) +
             Bytes(dp).pack(0x02) +
             Bytes(dq).pack(0x02) +
             Bytes(qinv).pack(0x02);
    char *str = bytes.pack(0x30).base64();
    std::printf("-----BEGIN RSA PRIVATE KEY-----");

    for (int i = 0; str[i] != '\0'; ++i)
        if (i % 64)
            std::printf("%c", str[i]);
        else
            std::printf("\n%c", str[i]);

    std::printf("\n-----END RSA PRIVATE KEY-----");
#ifndef _WIN32
    std::printf("\n");
#endif
    delete[] str;
    return 0;
}