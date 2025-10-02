#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

// Function to compute gcd of two numbers
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Function to compute base^expo mod m
int power(int base, int expo, int m) {
    long long res = 1;
    base = base % m;
    while (expo > 0) {
        if (expo & 1) res = (res * base) % m;
        base = ((long long)base * base) % m;
        expo = expo / 2;
    }
    return (int)res;
}

// Function to find modular inverse of e modulo phi(n)
int modInverse(int e, int phi) {
    for (int d = 2; d < phi; d++) {
        if ((e * d) % phi == 1)
            return d;
    }
    return -1;
}

// RSA Key Generation
void generateKeys(int *e, int *d, int *n) {
    int p = 7919;
    int q = 1009;
    
    *n = p * q;
    int phi = (p - 1) * (q - 1);

    // Choose e, where 1 < e < phi(n) and gcd(e, phi(n)) == 1
    for (*e = 2; *e < phi; (*e)++) {
        if (gcd(*e, phi) == 1)
            break;
    }

    // Compute d such that e * d ≡ 1 (mod phi(n))
    *d = modInverse(*e, phi);
}

// Encrypt message using public key (e, n)
int encrypt(int m, int e, int n) {
    return power(m, e, n);
}

// Decrypt message using private key (d, n)
int decrypt(int c, int d, int n) {
    return power(c, d, n);
}

int main() {
    int e, d, n;
    
    generateKeys(&e, &d, &n);
  
    printf("Public Key (e, n): (%d, %d)\n", e, n);
    printf("Private Key (d, n): (%d, %d)\n", d, n);

    int M = 123;
    printf("Original Message: %d\n", M);

    int C = encrypt(M, e, n);
    printf("Encrypted Message: %d\n", C);

    int decrypted = decrypt(C, d, n);
    printf("Decrypted Message: %d\n", decrypted);

    return 0;
}
