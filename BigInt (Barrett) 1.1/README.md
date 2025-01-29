# Barrett Reduction based Big Integer Calculator Suite

## 1. Installation
### Windows
 1. Download these 4 files: `bigint.dll`, `asn1.dll`, `pow0x.exe`, `keygen.exe`.
### Windows (Manual Building)
 1. Download and install TDM-GCC 64-bit compiler from [this site](https://jmeubank.github.io/tdm-gcc/download/).
 2. Download this entire folder (`BigInt (Barrett) 1.1`) with source codes.
 3. Run `.bat` files (automated build script) in this order:
   `make_bigint.bat` -> `make_asn1.bat` -> `make_pow0x.bat` -> `make_keygen.bat`
### Linux
 1. clone the git repository via this command.
   `git clone https://github.com/8059542577066/C-CC-RSA-Related-Tools.git`
 2. Navigate into the project.
   `cd BigInt\ (Barrett)\ 1.1`
 3. Build the project using these commands. Make sure that your GCC compiler is version >= 5.1.0 (c++11 support).
   `g++ bigint.cc -O3 -c`
   `g++ asn1.cc -O3 -c`
   `g++ bigint.o pow0x.cc -O3 -o pow0x`
   `g++ bigint.o asn1.o keygen.cc -O3 -o keygen`

## (2) pow0x
 This program calculates **modular exponentiation** in hexadecimal. Modular exponentiation is the key algorithm used in Diffie-Hellman key exchange and also in RSA public key crypto system. I built this program first to make sure that my custom `BigInt` class can perform necessary calculations for RSA fast enough.
 Use it like below. Since the program already assumes that the input data are already in hexadecimal format, you shouldn't use `0x` prefixes for integers.
   `pow0x a0a90b8b6eb6280adf5987a519fe1d058e33ce8bd81277248b4f66cdbb15e81d0bb84be45f70dcdaaaaa09417d19c71d1051a53dc8fc76937c13b98aa90148c8138d23ccc6e4d30b8786a708fba2839eca69f980203aaab7d24c9bd6769c956046bd54b87e258b3e961162015ba230a7569e5d8819b4f67567c9307d9363d7bd 846b6f1c413c62f360b513bb8962d472e58e029adcc8af6292443c728770f9680f9c164ee22095d90dea2262f279fc33462d9de2153a6d6efdb8e95b51a083a2ef93775def986b695e05d6444b82ad1b9e8c4d99e8269e932428ead92ca6f5003197c825e4dd704f666ef46adab1aba6ba09a755d20414d9824ff375ea5498b0 834cee5d25e4620d8c3ae795a9ac9c747b39123d89e59d34ed66b2ff6659b7860f8cfc6ef8c98dab872d068bf0d68a87456ba8d4fdd83e444efb9b50aa0fe7b519aa2e5e5a3fffb3257544b699a31d1f22521243b578c9b46d11fc1ffc37badd7e10f0920d6dc028a83ffebdeb2ff62391f3a3a1ab1331129676cef44528d8b1`
 Above example calculates modular exponentiation of 3 random 1024-bit hexadecimal integers.

 There is `check_pow0x_win.py` script that measures the execution time for certain input size. In order to use this script in Linux, please make necessary changes in the script, as it was created for Windows usage only.

## (3) keygen
 This is the **RSA private key generator**, the ultimate purpose of this project. The main difference from `openssl genrsa` module is that, with this program you can customize one of the prime numbers yourself! So instead of relying on black box pseudo random number generator of `openssl`, you have the full control over randomness of the prime numbers.
 Use it like this:
   `keygen 2048 facefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefacefaceface`
 It's a rather goofy example, but I wanted to prove my point. The key generated with above command will have one of its prime number to be adjacent to that repeating `face` number.
   ![image](https://github.com/user-attachments/assets/dd8051c7-83e5-4592-a148-a79ebf277dee)
   ![image](https://github.com/user-attachments/assets/8e639945-c0fc-454f-991b-fb2867123819)
 We can confirm with `openssl rsa` module that the private key output was indeed a valid RSA private key with the intended key size.
   ![image](https://github.com/user-attachments/assets/19866b03-9edc-430c-9384-2ca758a7e97d)
 And this proves that the key indeed has a direct prime number input we wanted it to use. Because the repeating `face` integer wasn't prime, the program automatically calculated the next prime number after it.

 Of course in real environments where your privacy matters, don't seed your prime number like that. I recommend using pseudo random number output (like that of Python's `random.getrandbits()` function), copy & paste it into the argument, then manaully tweak some digits.
