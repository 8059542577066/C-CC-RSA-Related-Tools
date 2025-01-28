#include "asn1.hh"
#include "bigint.hh"


Bytes::Bytes(int cap)
    : cap(cap),
      arr(new byte[this->cap]) {}


char Bytes::base64(byte ord) const
{
    if (ord < 26)
        return ord + 0x41;
    else if (ord < 52)
        return ord + 0x47;
    else if (ord < 62)
        return ord - 0x04;
    else if (ord < 63)
        return 0x2B;
    else
        return 0x2F;
}


Bytes::Bytes()
    : cap(1), len(1),
      arr(new byte[1])
{
    this->arr[0] = 0;
}

Bytes::Bytes(const BigInt &n)
{
    char *c_str = n.c_str();
    int str_len = 0;

    while (c_str[str_len] != '\0')
        ++str_len;

    if (str_len % 2 > 0)
        this->cap = str_len / 2 + 1;
    else
        this->cap = str_len / 2;

    this->len = this->cap;
    this->arr = new byte[this->cap];
    int i, j = 0;
    byte ord;

    for (i = str_len - 1; i > 0; i -= 2)
    {
        if (c_str[i - 1] < 0x40)
            ord = c_str[i - 1] - 0x30;
        else if (c_str[i - 1] < 0x60)
            ord = c_str[i - 1] - 0x37;
        else
            ord = c_str[i - 1] - 0x57;

        ord <<= 4;

        if (c_str[i] < 0x40)
            ord += c_str[i] - 0x30;
        else if (c_str[i] < 0x60)
            ord += c_str[i] - 0x37;
        else
            ord += c_str[i] - 0x57;

        this->arr[j++] = ord;
    }

    if (i == 0)
    {
        if (c_str[i] < 0x40)
            ord = c_str[i] - 0x30;
        else if (c_str[i] < 0x60)
            ord = c_str[i] - 0x37;
        else
            ord = c_str[i] - 0x57;

        this->arr[j++] = ord;
    }

    while (this->arr[--j] == 0 && this->len > 1)
        --this->len;

    for (i = this->len - 1, j = 0; i > j; --i, ++j)
    {
        this->arr[i] ^= this->arr[j];
        this->arr[j] ^= this->arr[i];
        this->arr[i] ^= this->arr[j];
    }

    delete[] c_str;
}

Bytes::~Bytes()
{
    delete[] this->arr;
}


Bytes &Bytes::operator+=(const Bytes &other)
{
    if (this->cap < this->len + other.len)
    {
        this->cap = this->len + other.len;
        byte *ptr = new byte[this->cap];

        for (int i = 0; i < this->len; ++i)
            ptr[i] = this->arr[i];

        delete[] this->arr;
        this->arr = ptr;
    }

    for (int i = 0; i < other.len; ++i)
        this->arr[i + this->len] = other.arr[i];

    this->len += other.len;
    return *this;
}

Bytes Bytes::operator+(const Bytes &other) const
{
    Bytes res(this->len + other.len);
    res.len = this->len + other.len;

    for (int i = 0; i < this->len; ++i)
        res.arr[i] = this->arr[i];

    for (int i = 0; i < other.len; ++i)
        res.arr[i + this->len] = other.arr[i];

    return res;
}


Bytes Bytes::pack(byte type) const
{
    Bytes res(this->len + 6);
    int new_len = this->len, i = 0;
    res.arr[i++] = type;

    if (type == 0x02 && (this->arr[0] & 0x80) > 0)
        ++new_len;

    if (new_len <= 0x7F)
        res.arr[i++] = new_len;
    else if (new_len <= 0xFF)
        res.arr[i++] = 0x81,
        res.arr[i++] = new_len;
    else if (new_len <= 0xFFFF)
        res.arr[i++] = 0x82,
        res.arr[i++] = new_len >> 8,
        res.arr[i++] = new_len;
    else if (new_len <= 0xFFFFFF)
        res.arr[i++] = 0x83,
        res.arr[i++] = new_len >> 16,
        res.arr[i++] = new_len >> 8,
        res.arr[i++] = new_len;

    if (type == 0x02 && (this->arr[0] & 0x80) > 0)
        res.arr[i++] = 0x00;

    for (int j = 0; j < this->len; ++j)
        res.arr[i++] = this->arr[j];

    res.len = i;
    return res;
}

char *Bytes::base64() const
{
    char *res = new char[this->len * 3 / 2];
    int i = 0, j = 0, t;

    while (i < this->len - 3)
        t = (int)this->arr[i] << 16 |
            (int)this->arr[i + 1] << 8 |
            (int)this->arr[i + 2],
        res[j + 3] = this->base64(t % 64), t >>= 6,
        res[j + 2] = this->base64(t % 64), t >>= 6,
        res[j + 1] = this->base64(t % 64), t >>= 6,
        res[j] = this->base64(t),
        i += 3, j += 4;

    if (this->len % 3 == 1)
        t = (int)this->arr[i] << 16,
        res[j + 3] = '=',
        res[j + 2] = '=', t >>= 12,
        res[j + 1] = this->base64(t % 64), t >>= 6,
        res[j] = this->base64(t);
    else if (this->len % 3 == 2)
        t = (int)this->arr[i] << 16 |
            (int)this->arr[i + 1] << 8,
        res[j + 3] = '=', t >>= 6,
        res[j + 2] = this->base64(t % 64), t >>= 6,
        res[j + 1] = this->base64(t % 64), t >>= 6,
        res[j] = this->base64(t);
    else
        t = (int)this->arr[i] << 16 |
            (int)this->arr[i + 1] << 8 |
            (int)this->arr[i + 2],
        res[j + 3] = this->base64(t % 64), t >>= 6,
        res[j + 2] = this->base64(t % 64), t >>= 6,
        res[j + 1] = this->base64(t % 64), t >>= 6,
        res[j] = this->base64(t);

    res[j + 4] = '\0';
    return res;
}