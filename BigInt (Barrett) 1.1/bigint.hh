#ifndef BIGINT_HH
#define BIGINT_HH


#ifdef _WIN32
#ifdef BUILD_LIB
#define LIB_CLASS __declspec(dllexport)
#else
#define LIB_CLASS __declspec(dllimport)
#endif
#else
#define LIB_CLASS
#endif

#ifndef N_ARY
#define N_ARY 5
#endif

#ifndef KARA_CUT
#define KARA_CUT 72
#endif


#ifndef SUB_X_LT_Y
#define SUB_X_LT_Y 1
#endif

#ifndef SH_BY_NEG
#define SH_BY_NEG 2
#endif

#ifndef MOD_DIV_BY_0
#define MOD_DIV_BY_0 3
#endif

#ifndef N_ARY_ERR
#define N_ARY_ERR 4
#endif

#ifndef INVALID_INPUT
#define INVALID_INPUT 5
#endif


typedef unsigned long long uint64;

class LIB_CLASS BigInt
{
    int cap, len;
    uint64 *arr;

    BigInt(int cap);
    BigInt(uint64 *ptr, int len);

    BigInt &operator*=(uint64 val);
    BigInt operator*(uint64 val) const;

    void __add(const BigInt &other, int pad);
    void __init(const BigInt &m);
    void __kara(const BigInt &x, const BigInt &y);
    void __mul(const BigInt &x, const BigInt &y,
               const BigInt &m, const BigInt &q,
               BigInt &temp1, BigInt &temp2, bool first);

public:
    BigInt();
    BigInt(const BigInt &other);
    BigInt(BigInt &&other);
    ~BigInt();

    bool operator==(const BigInt &other) const;
    bool operator!=(const BigInt &other) const;
    bool operator<(const BigInt &other) const;
    bool operator<=(const BigInt &other) const;
    bool operator>(const BigInt &other) const;
    bool operator>=(const BigInt &other) const;

    BigInt &operator=(const BigInt &other);
    BigInt &operator=(BigInt &&other);
    BigInt &operator=(uint64);

    BigInt &operator+=(const BigInt &other);
    BigInt operator+(const BigInt &other) const;
    BigInt &operator-=(const BigInt &other);
    BigInt operator-(const BigInt &other) const;

    BigInt &operator<<=(int by);
    BigInt operator<<(int by) const;
    BigInt &operator>>=(int by);
    BigInt operator>>(int by) const;

    BigInt &operator*=(const BigInt &other);
    BigInt operator*(const BigInt &other) const;

    BigInt &operator%=(const BigInt &other);
    BigInt operator%(const BigInt &other) const;
    BigInt &operator/=(const BigInt &other);
    BigInt operator/(const BigInt &other) const;

    static BigInt pow(const BigInt &b, BigInt e,
                      const BigInt &m);
    static BigInt inv(const BigInt &a, const BigInt &n);

    int bits() const;
    void scan(const char *c_str);
    char *c_str() const;
    void shrink_to_fit();
};


#endif