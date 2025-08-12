#include "bigint.hh"


BigInt::BigInt(int cap)
    : cap(cap > 0 ? cap : 1),
      arr(new uint64[this->cap]) {}

BigInt::BigInt(uint64 *ptr, int len)
    : cap(0), len(len),
      arr(ptr) {}


BigInt &BigInt::operator*=(uint64 val)
{
    if (val == 0)
    {
        this->len = 1, this->arr[0] = 0;
        return *this;
    }

    if (this->cap < this->len + 1)
    {
        this->cap = this->len + 1;
        uint64 *ptr = new uint64[this->cap];

        for (int i = 0; i < this->len; ++i)
            ptr[i] = this->arr[i];

        delete[] this->arr;
        this->arr = ptr;
    }

    __uint128_t t;
    uint64 c = 0;

    for (int i = 0; i < this->len; ++i)
        t = (__uint128_t)this->arr[i] * val + c,
        this->arr[i] = t, c = t >> 64;

    if (c > 0)
        this->arr[this->len++] = c;

    return *this;
}

BigInt BigInt::operator*(uint64 val) const
{
    if (val == 0)
        return BigInt();

    BigInt res(this->len + 1);
    __uint128_t t;
    uint64 c = 0;

    for (int i = 0; i < this->len; ++i)
        t = (__uint128_t)this->arr[i] * val + c,
        res.arr[i] = t, c = t >> 64;

    res.len = this->len;

    if (c > 0)
        res.arr[res.len++] = c;

    return res;
}


void BigInt::__add(const BigInt &other, int pad)
{
    if (other.len == 1 && other.arr[0] == 0)
        return;

    for (int i = this->len; i < pad; ++i)
        this->arr[i] = 0;

    __uint128_t t;
    uint64 c = 0;
    int i;

    for (i = pad; i < this->len; ++i)
        t = (__uint128_t)this->arr[i] + other.arr[i - pad] + c,
        this->arr[i] = t, c = t >> 64;

    if (this->len < other.len + pad)
        for (i -= pad; i < other.len; ++i)
            t = other.arr[i] + c,
            this->arr[i + pad] = t, c = t >> 64;

    this->len = other.len + pad;

    if (c > 0)
        this->arr[this->len++] = c;
}

void BigInt::__init(const BigInt &m, BigInt &q) const
{
    q = 1, q <<= m.bits() * 2, q /= m;
}

void BigInt::__kara(const BigInt &x, const BigInt &y)
{
    const BigInt *s, *b;

    if (x.len < y.len)
        s = &x, b = &y;
    else
        s = &y, b = &x;

    if (b->len <= KARA_CUT)
        *this = x * y;
    else
    {
        int b1_top = b->len - 1,
            b0_top = b1_top / 2, b1_bot = b0_top + 1;

        if (s->len > b1_bot)
        {
            int s1_top = s->len - 1,
                s0_top = b0_top, s1_bot = s0_top + 1;
            BigInt b0(b->arr, b0_top + 1),
                b1(&b->arr[b1_bot], b1_top - b1_bot + 1),
                s0(s->arr, s0_top + 1),
                s1(&s->arr[s1_bot], s1_top - s1_bot + 1),
                temp1, temp2, temp3;

            while (b0.arr[b0.len - 1] == 0 && b0.len > 1)
                --b0.len;

            while (s0.arr[s0.len - 1] == 0 && s0.len > 1)
                --s0.len;

            temp1.__kara(b1, s1), temp2.__kara(b0, s0);
            temp3.__kara(b1 + b0, s1 + s0);
            b0.arr = nullptr, b1.arr = nullptr;
            s0.arr = nullptr, s1.arr = nullptr;
            temp3 -= temp1, temp3 -= temp2;
            *this = BigInt(b->len + s->len), *this = temp2;
            this->__add(temp3, b1_bot);
            this->__add(temp1, b1_bot * 2);
        }
        else
        {
            int s0_top = s->len - 1;
            BigInt b0(b->arr, b0_top + 1),
                b1(&b->arr[b1_bot], b1_top - b1_bot + 1),
                s0(s->arr, s0_top + 1),
                temp1, temp2;

            while (b0.arr[b0.len - 1] == 0 && b0.len > 1)
                --b0.len;

            while (s0.arr[s0.len - 1] == 0 && s0.len > 1)
                --s0.len;

            temp1.__kara(b1, s0), temp2.__kara(b0, s0);
            b0.arr = nullptr, b1.arr = nullptr;
            s0.arr = nullptr;
            *this = BigInt(b->len + s->len), *this = temp2;
            this->__add(temp1, b1_bot);
        }
    }
}

void BigInt::__mul(const BigInt &x, const BigInt &y,
                   const BigInt &m, const BigInt &q,
                   BigInt &temp1, BigInt &temp2, bool first)
{
    if (first)
    {
        this->__kara(x, y), *this %= m;
        return;
    }

    temp1.__kara(x, y);
    temp2.__kara(temp1, q), temp2 >>= m.bits() * 2;
    temp2.__kara(temp2, m), temp1 -= temp2;
    *this = temp1 < m ? temp1 : temp1 -= m;
}


BigInt::BigInt()
    : cap(1), len(1),
      arr(new uint64[1])
{
    this->arr[0] = 0;
}

BigInt::BigInt(const BigInt &other)
    : cap(other.len), len(other.len),
      arr(new uint64[other.len])
{
    for (int i = 0; i < this->len; ++i)
        this->arr[i] = other.arr[i];
}

BigInt::BigInt(BigInt &&other)
    : cap(other.cap), len(other.len),
      arr(other.arr)
{
    other.arr = nullptr;
}

BigInt::~BigInt()
{
    delete[] this->arr;
}


bool BigInt::operator==(const BigInt &other) const
{
    if (this->len != other.len)
        return false;

    for (int i = this->len - 1; i > -1; --i)
        if (this->arr[i] != other.arr[i])
            return false;

    return true;
}

bool BigInt::operator!=(const BigInt &other) const
{
    if (this->len != other.len)
        return true;

    for (int i = this->len - 1; i > -1; --i)
        if (this->arr[i] != other.arr[i])
            return true;

    return false;
}

bool BigInt::operator<(const BigInt &other) const
{
    if (this->len != other.len)
        return this->len < other.len;

    for (int i = this->len - 1; i > -1; --i)
        if (this->arr[i] != other.arr[i])
            return this->arr[i] < other.arr[i];

    return false;
}

bool BigInt::operator<=(const BigInt &other) const
{
    if (this->len != other.len)
        return this->len < other.len;

    for (int i = this->len - 1; i > -1; --i)
        if (this->arr[i] != other.arr[i])
            return this->arr[i] < other.arr[i];

    return true;
}

bool BigInt::operator>(const BigInt &other) const
{
    if (this->len != other.len)
        return this->len > other.len;

    for (int i = this->len - 1; i > -1; --i)
        if (this->arr[i] != other.arr[i])
            return this->arr[i] > other.arr[i];

    return false;
}

bool BigInt::operator>=(const BigInt &other) const
{
    if (this->len != other.len)
        return this->len > other.len;

    for (int i = this->len - 1; i > -1; --i)
        if (this->arr[i] != other.arr[i])
            return this->arr[i] > other.arr[i];

    return true;
}

BigInt &BigInt::operator=(const BigInt &other)
{
    if (this->cap < other.len)
    {
        delete[] this->arr;
        this->cap = other.len;
        this->arr = new uint64[this->cap];
    }

    this->len = other.len;

    for (int i = 0; i < this->len; ++i)
        this->arr[i] = other.arr[i];

    return *this;
}

BigInt &BigInt::operator=(BigInt &&other)
{
    delete this->arr;
    this->cap = other.cap, this->len = other.len;
    this->arr = other.arr, other.arr = nullptr;
    return *this;
}

BigInt &BigInt::operator=(uint64 val)
{
    this->len = 1, this->arr[0] = val;
    return *this;
}

BigInt &BigInt::operator+=(const BigInt &other)
{
    int sml, big, i;
    const BigInt *b;
    __uint128_t t;
    uint64 c = 0;

    if (this->len < other.len)
        sml = this->len, big = other.len, b = &other;
    else
        sml = other.len, big = this->len, b = this;

    if (this->cap < big + 1)
    {
        this->cap = big + 1;
        uint64 *ptr = new uint64[this->cap];

        for (i = 0; i < this->len; ++i)
            ptr[i] = this->arr[i];

        delete[] this->arr;
        this->arr = ptr;
    }

    for (i = 0; i < sml; ++i)
        t = (__uint128_t)this->arr[i] + other.arr[i] + c,
        this->arr[i] = t, c = t >> 64;

    while (i < big)
        t = (__uint128_t)b->arr[i] + c,
        this->arr[i++] = t, c = t >> 64;

    this->len = big;

    if (c > 0)
        this->arr[this->len++] = c;

    return *this;
}

BigInt BigInt::operator+(const BigInt &other) const
{
    int sml, big, i;
    const BigInt *b;
    __uint128_t t;
    uint64 c = 0;

    if (this->len < other.len)
        sml = this->len, big = other.len, b = &other;
    else
        sml = other.len, big = this->len, b = this;

    BigInt res(big + 1);

    for (i = 0; i < sml; ++i)
        t = (__uint128_t)this->arr[i] + other.arr[i] + c,
        res.arr[i] = t, c = t >> 64;

    while (i < big)
        t = (__uint128_t)b->arr[i] + c,
        res.arr[i++] = t, c = t >> 64;

    res.len = big;

    if (c > 0)
        res.arr[res.len++] = c;

    return res;
}

BigInt &BigInt::operator-=(const BigInt &other)
{
    if (*this < other)
        throw SUB_X_LT_Y;

    __uint128_t t;
    uint64 c = 0;
    int i;

    for (i = 0; i < other.len; ++i)
        t = (__uint128_t)this->arr[i] - other.arr[i] - c,
        this->arr[i] = t, c = t << 63 >> 127;

    while (i < this->len)
        t = (__uint128_t)this->arr[i] - c,
        this->arr[i++] = t, c = t << 63 >> 127;

    while (this->arr[--i] == 0 && this->len > 1)
        --this->len;

    return *this;
}

BigInt BigInt::operator-(const BigInt &other) const
{
    if (*this < other)
        throw SUB_X_LT_Y;

    BigInt res(this->len);
    __uint128_t t;
    uint64 c = 0;
    int i;

    for (i = 0; i < other.len; ++i)
        t = (__uint128_t)this->arr[i] - other.arr[i] - c,
        res.arr[i] = t, c = t << 63 >> 127;

    while (i < this->len)
        t = (__uint128_t)this->arr[i] - c,
        res.arr[i++] = t, c = t << 63 >> 127;

    res.len = this->len;

    while (res.arr[--i] == 0 && res.len > 1)
        --res.len;

    return res;
}

BigInt &BigInt::operator<<=(int by)
{
    if (by < 0)
        throw SH_BY_NEG;
    else if (this->len == 1 && this->arr[0] == 0)
        return *this;

    int quo = by / 64, rem = by % 64,
        del_len = quo + (rem > 0 ? 1 : 0);

    if (this->cap < this->len + del_len)
    {
        this->cap = this->len + del_len;
        uint64 *ptr = new uint64[this->cap];

        for (int i = 0; i < this->len; ++i)
            ptr[i] = this->arr[i];

        delete[] this->arr;
        this->arr = ptr;
    }

    if (rem > 0)
    {
        const uint64 MASK_L = (uint64)-1 << (64 - rem);
        int i = this->len++;
        this->arr[i] = (this->arr[i - 1] & MASK_L) >> (64 - rem);

        for (--i; i > 0; --i)
            this->arr[i] = (this->arr[i - 1] & MASK_L) >> (64 - rem) |
                           this->arr[i] << rem;

        this->arr[0] <<= rem;

        if (this->arr[this->len - 1] == 0)
            --this->len;
    }

    if (quo > 0)
    {
        for (int i = this->len - 1; i > -1; --i)
            this->arr[i + quo] = this->arr[i];

        for (int i = quo - 1; i > -1; --i)
            this->arr[i] = 0;
    }

    this->len += quo;
    return *this;
}

BigInt BigInt::operator<<(int by) const
{
    if (by < 0)
        throw SH_BY_NEG;
    else if (this->len == 1 && this->arr[0] == 0)
        return *this;

    int quo = by / 64, rem = by % 64,
        del_len = quo + (rem > 0 ? 1 : 0);
    BigInt res(this->len + del_len);
    res = *this;

    if (rem > 0)
    {
        const uint64 MASK_L = (uint64)-1 << (64 - rem);
        int i = res.len++;
        res.arr[i] = (res.arr[i - 1] & MASK_L) >> (64 - rem);

        for (--i; i > 0; --i)
            res.arr[i] = (res.arr[i - 1] & MASK_L) >> (64 - rem) |
                         res.arr[i] << rem;

        res.arr[0] <<= rem;

        if (res.arr[res.len - 1] == 0)
            --res.len;
    }

    if (quo > 0)
    {
        for (int i = res.len - 1; i > -1; --i)
            res.arr[i + quo] = res.arr[i];

        for (int i = quo - 1; i > -1; --i)
            res.arr[i] = 0;
    }

    res.len += quo;
    return res;
}

BigInt &BigInt::operator>>=(int by)
{
    if (by < 0)
        throw SH_BY_NEG;
    else if (this->bits() <= by)
    {
        this->len = 1, this->arr[0] = 0;
        return *this;
    }

    int quo = by / 64, rem = by % 64;

    if (rem == 0)
    {
        for (int i = 0; i + quo < this->len; ++i)
            this->arr[i] = this->arr[i + quo];

        this->len -= quo;
        return *this;
    }

    const uint64 MASK_R = (uint64)-1 >> (64 - rem);
    int i;

    for (i = 0; i + quo < this->len - 1; ++i)
        this->arr[i] = (this->arr[i + quo + 1] & MASK_R) << (64 - rem) |
                       this->arr[i + quo] >> rem;

    this->arr[i] = this->arr[i + quo] >> rem;
    this->len -= quo;

    if (this->arr[i] == 0)
        --this->len;

    return *this;
}

BigInt BigInt::operator>>(int by) const
{
    if (by < 0)
        throw SH_BY_NEG;
    else if (this->bits() <= by)
        return BigInt();

    int quo = by / 64, rem = by % 64;
    BigInt res(this->len - quo);

    if (rem == 0)
    {
        for (int i = 0; i + quo < this->len; ++i)
            res.arr[i] = this->arr[i + quo];

        res.len = this->len - quo;
        return res;
    }

    const uint64 MASK_R = (uint64)-1 >> (64 - rem);
    int i;

    for (i = 0; i + quo < this->len - 1; ++i)
        res.arr[i] = (this->arr[i + quo + 1] & MASK_R) << (64 - rem) |
                     this->arr[i + quo] >> rem;

    res.arr[i] = this->arr[i + quo] >> rem;
    res.len = this->len - quo;

    if (res.arr[i] == 0)
        --res.len;

    return res;
}

BigInt &BigInt::operator*=(const BigInt &other)
{
    BigInt res(this->len + other.len), t(this->len + 1);
    res.len = 1, res.arr[0] = 0;

    for (int i = 0; i < other.len; ++i)
        t = *this, t *= other.arr[i], res.__add(t, i);

    return *this = res;
}

BigInt BigInt::operator*(const BigInt &other) const
{
    BigInt res(this->len + other.len), t(this->len + 1);
    res.len = 1, res.arr[0] = 0;

    for (int i = 0; i < other.len; ++i)
        t = *this, t *= other.arr[i], res.__add(t, i);

    return res;
}

BigInt &BigInt::operator%=(const BigInt &other)
{
    if (other.len == 1 && other.arr[0] == 0)
        throw MOD_DIV_BY_0;
    else if (*this < other)
        return *this;

    BigInt t = other;
    t <<= this->bits() - other.bits();

    for (t = *this < t ? t >> 1 : t; *this >= other; t >>= 1)
        if (*this >= t)
            *this -= t;

    return *this;
}

BigInt BigInt::operator%(const BigInt &other) const
{
    if (other.len == 1 && other.arr[0] == 0)
        throw MOD_DIV_BY_0;
    else if (*this < other)
        return *this;

    BigInt res = *this, t = other;
    t <<= res.bits() - other.bits();

    if (res < t)
        t >>= 1;

    for (t = res < t ? t >> 1 : t; res >= other; t >>= 1)
        if (res >= t)
            res -= t;

    return res;
}

BigInt &BigInt::operator/=(const BigInt &other)
{
    if (other.len == 1 && other.arr[0] == 0)
        throw MOD_DIV_BY_0;
    else if (*this < other)
    {
        this->len = 1, this->arr[0] = 0;
        return *this;
    }

    BigInt res, t = other, p;
    t <<= this->bits() - other.bits();
    p.arr[0] = 1, p <<= this->bits() - other.bits();

    if (*this < t)
        t >>= 1, p >>= 1;

    while (*this >= other)
    {
        if (*this >= t)
            *this -= t, res += p;

        t >>= 1, p >>= 1;
    }

    return *this = res;
}

BigInt BigInt::operator/(const BigInt &other) const
{
    if (other.len == 1 && other.arr[0] == 0)
        throw MOD_DIV_BY_0;
    else if (*this < other)
        return BigInt();

    BigInt res, that = *this, t = other, p;
    t <<= that.bits() - other.bits();
    p.arr[0] = 1, p <<= that.bits() - other.bits();

    if (that < t)
        t >>= 1, p >>= 1;

    while (that >= other)
    {
        if (that >= t)
            that -= t, res += p;

        t >>= 1, p >>= 1;
    }

    return res;
}


BigInt BigInt::pow(BigInt b, BigInt e, const BigInt &m)
{
    if (m.len == 1 && m.arr[0] == 0)
        throw MOD_DIV_BY_0;
    else if (N_ARY < 4 || N_ARY > 16)
        throw N_ARY_ERR;
    else if (m.len == 1 && m.arr[0] == 1)
        return BigInt();

    BigInt res, zero, q, temp1, temp2,
        *b_pow = new BigInt[1 << N_ARY];
    res.arr[0] = 1, res.__init(m, q), b_pow[0].arr[0] = 1;
    b_pow[1].__mul(b_pow[0], b, m, q, temp1, temp2, true);
    unsigned *e_arr = new unsigned[e.bits() / N_ARY + 1];
    int i;

    for (i = 2; i < 1 << N_ARY; ++i)
        b_pow[i].__mul(b_pow[i - 1], b_pow[1], m, q, temp1, temp2, false);

    for (i = 0; e > zero; e >>= N_ARY)
        e_arr[i++] = e.arr[0] & ((1 << N_ARY) - 1);

    for (--i; i > -1; --i)
    {
        for (int j = 0; j < N_ARY; ++j)
            res.__mul(res, res, m, q, temp1, temp2, false);

        res.__mul(res, b_pow[e_arr[i]], m, q, temp1, temp2, false);
    }

    delete[] b_pow, delete[] e_arr;
    return res;
}

BigInt BigInt::inv(const BigInt &a, const BigInt &n)
{
    BigInt t, new_t, r = n, new_r = a, zero,
                     q, temp, term1, term2;
    new_t.arr[0] = 1;
    int t_sgn = 1, new_t_sgn = 1, temp_sgn;

    while (new_r > zero)
    {
        q = r / new_r;

        if (t_sgn * new_t_sgn < 0)
            temp = t + q * new_t, temp_sgn = t_sgn > 0 ? 1 : -1,
            t = new_t, t_sgn = new_t_sgn,
            new_t = temp, new_t_sgn = temp_sgn;
        else
        {
            temp_sgn = 1;

            if (t_sgn > 0)
                term1 = t, term2 = q * new_t;
            else
                term1 = q * new_t, term2 = t;

            if (term1 < term2)
                temp = term1, term1 = term2, term2 = temp,
                temp_sgn *= -1;

            temp = term1 - term2;
            t = new_t, t_sgn = new_t_sgn;
            new_t = temp, new_t_sgn = temp_sgn;
        }

        temp = r - q * new_r, r = new_r, new_r = temp;
    }

    return t_sgn < 0 ? n - t : t;
}

int BigInt::bits() const
{
    if (this->len == 1 && this->arr[0] == 0)
        return 0;

    int full = this->len - 1, by = 32, gap = 16;

    while (true)
    {
        uint64 num = this->arr[this->len - 1] >> by;

        if (num == 1)
            return 64 * full + by + 1;
        else if (gap == 0)
            return 64 * full + 1;
        else if (num > 0)
            by += gap;
        else
            by -= gap;

        gap /= 2;
    }
}

void BigInt::scan(const char *c_str)
{
    int str_len;

    for (str_len = 0; c_str[str_len] != '\0'; ++str_len)
        if (!((c_str[str_len] >= '0' && c_str[str_len] <= '9') ||
              (c_str[str_len] >= 'A' && c_str[str_len] <= 'F') ||
              (c_str[str_len] >= 'a' && c_str[str_len] <= 'f')))
            throw INVALID_INPUT;

    BigInt res, power;
    this->len = 1, this->arr[0] = 0, power.arr[0] = 1;

    for (int i = str_len - 1; i > -1; --i)
    {
        uint64 ord;

        if (c_str[i] < 0x40)
            ord = c_str[i] - 0x30;
        else if (c_str[i] < 0x60)
            ord = c_str[i] - 0x37;
        else
            ord = c_str[i] - 0x57;

        *this += power * ord, power <<= 4;
    }
}

char *BigInt::c_str() const
{
    if (this->len == 1 && this->arr[0] == 0)
    {
        char *res = new char[2];
        res[0] = '0', res[1] = '\0';
        return res;
    }

    BigInt that = *this, zero;
    char *res = new char[that.len * 16 + 1];
    const char lookup[16] =
        {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
         'A', 'B', 'C', 'D', 'E', 'F'};
    int i, j;

    for (i = 0; that > zero; that >>= 4)
        res[i++] = lookup[that.arr[0] & 0xF];

    for (res[i--] = '\0', j = 0; i > j; --i, ++j)
        res[i] ^= res[j], res[j] ^= res[i], res[i] ^= res[j];

    return res;
}

void BigInt::shrink_to_fit()
{
    if (this->cap > this->len)
    {
        this->cap = this->len;
        uint64 *ptr = new uint64[this->cap];

        for (int i = 0; i < this->len; ++i)
            ptr[i] = this->arr[i];

        delete[] this->arr;
        this->arr = ptr;
    }
}