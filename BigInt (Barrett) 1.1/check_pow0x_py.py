import random
import time
import subprocess


rand = random.getrandbits

def rands(n):
    return rand(n), rand(n), rand(n)

def makecmd(b, e, m):
    return "python pow0x.py " + hex(b)[2:] + " " +\
           hex(e)[2:] + " " + hex(m)[2:]

def check(b, e, m):
    time1 = time.time()
    result = subprocess.check_output(makecmd(b, e, m))
    time2 = time.time()
    return time2 - time1


if __name__ == "__main__":
    size = int(input("Bit Size: "))
    repeat = int(input("Repeat: "))
    total_secs = 0
    for i in range(repeat):
        b, e, m = rands(size)
        secs = check(b, e, m)
        print("  Test " + str(i + 1) + ": Done")
        total_secs += secs
    print("On average, " + str(size) + "-bit tests took " +\
          str(round(total_secs / repeat, 3)) + " secs")
    input()
