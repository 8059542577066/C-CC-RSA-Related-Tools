import sys


if __name__ == "__main__":
    b = int(sys.argv[1], 16)
    e = int(sys.argv[2], 16)
    m = int(sys.argv[3], 16)
    print(hex(pow(b, e, m))[2:].upper())
