#include "bigint.hh"
#include <cctype>
#include <cstdio>


int check_str(const char *str)
{
    while (*str != '\0')
        if (!std::isxdigit(*str++))
            return INVALID_INPUT;

    return 0;
}

bool isall0(const char *str)
{
    while (*str != '\0')
        if (*str++ != '0')
            return false;

    return true;
}


int main(int argc, char **argv)
{
    if (argc != 4)
        std::printf(" *** Usage: pow0x [b] [e] [m]");
    else if (check_str(argv[1]) == INVALID_INPUT)
        std::printf(" *** ERROR: [b] not hex");
    else if (check_str(argv[2]) == INVALID_INPUT)
        std::printf(" *** ERROR: [e] not hex");
    else if (check_str(argv[3]) == INVALID_INPUT)
        std::printf(" *** ERROR: [m] not hex");
    else if (isall0(argv[3]))
        std::printf(" *** ERROR: [m] is 0");
    else
    {
        BigInt b, e, m, r;
        b.scan(argv[1]), e.scan(argv[2]), m.scan(argv[3]);
        r = BigInt::pow(b, e, m);
        char *str = r.c_str();
        std::printf(str);
        delete[] str;
    }

#ifndef _WIN32
    std::printf("\n");
#endif
    return 0;
}