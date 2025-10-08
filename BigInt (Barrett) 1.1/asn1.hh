#ifndef ASN1_HH
#define ASN1_HH


#ifdef _WIN32
#ifdef BUILD_LIB
#define LIB_CLASS __declspec(dllexport)
#else
#define LIB_CLASS __declspec(dllimport)
#endif
#else
#define LIB_CLASS
#endif


typedef unsigned char byte;
class BigInt;

class LIB_CLASS Bytes
{
    int cap, len;
    byte *arr;

    Bytes(int cap);

    char base64(byte ord) const;

public:
    Bytes();
    Bytes(const BigInt &n);
    ~Bytes();

    Bytes &operator+=(const Bytes &other);
    Bytes operator+(const Bytes &other) const;

    Bytes pack(byte type) const;
    char *base64() const;
};


#endif