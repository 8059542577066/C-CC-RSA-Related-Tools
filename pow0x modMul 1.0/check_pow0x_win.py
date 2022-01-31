import random
import time
import subprocess


rand = random.getrandbits

def rands(n):
    return rand(n - 1), rand(n), rand(n) | 1 << n - 1

def makecmd(b, e, m):
    return "pow0x " + hex(b)[2:] + " " +\
           hex(e)[2:] + " " + hex(m)[2:]

def check(b, e, m):
    time1 = time.time()
    result = subprocess.check_output(makecmd(b, e, m))
    time2 = time.time()
    return str(result)[2:-1] == hex(pow(b, e, m))[2:].upper(),\
           time2 - time1


if __name__ == "__main__":
    size = int(input("Bit Size: "))
    repeat = int(input("Repeat: "))
    checks = []
    total_secs = 0
    for i in range(repeat):
        b, e, m = rands(size)
        result, secs = check(b, e, m)
        print("  Test " + str(i + 1) + ": " + str(result))
        checks.append(result)
        total_secs += secs
    if all(checks):
        print("All " + str(repeat) + " tests successful")
        print("On average, " + str(size) + "-bit tests took " +\
              str(round(total_secs / repeat, 3)) + " secs")
    else:
        print("Some tests failed... T_T")
    input()
