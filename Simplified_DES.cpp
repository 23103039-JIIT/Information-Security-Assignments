#include <bits/stdc++.h>

using namespace std;

// P10 permutation table (1-based indexing)
const int P10[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};

// P8 permutation table
const int P8[8] = {6, 3, 7, 4, 8, 5, 10, 9};

// Initial Permutation IP for 8 bits
const int IP[8] = {2, 6, 3, 1, 4, 8, 5, 7};

// Inverse IP
const int IP_inv[8] = {4, 1, 3, 5, 7, 2, 8, 6};

// Expansion/Permutation EP (for 4 bits to 8 bits)
const int EP[8] = {4, 1, 2, 3, 2, 3, 4, 1};

// P4 permutation
const int P4[4] = {2, 4, 3, 1};

int S0[4][4] = {
    {1, 0, 3, 2},
    {3, 2, 1, 0},
    {0, 2, 1, 3},
    {3, 1, 3, 2}
};

int S1[4][4] = {
    {0, 1, 2, 3},
    {2, 0, 1, 3},
    {3, 0, 1, 0},
    {2, 1, 0, 3}
};

// Function to apply a permutation table to input bits (input stored in int)
int permute(int input, const int* table, int n, int input_len) {
    int output = 0;
    for(int i=0; i<n; i++) {
        // table[i] is 1-based index
        int bit = (input >> (input_len - table[i])) & 1;
        output = (output << 1) | bit;
    }
    return output;
}

// Left shift for 5-bit half, number of shifts = shifts
int leftShift5(int half, int shifts) {
    return ((half << shifts) & 0x1F) | (half >> (5 - shifts));
}

// Function to generate two 8-bit subkeys from 10-bit key
void keyGeneration(int key10, int* subkeys) {
    // Step 1: P10
    int permuted = permute(key10, P10, 10, 10);
    // Split into left and right 5 bits
    int left5 = (permuted >> 5) & 0x1F;
    int right5 = permuted & 0x1F;

    // LS-1
    left5 = leftShift5(left5, 1);
    right5 = leftShift5(right5, 1);

    // Combine halves and apply P8 -> Subkey1
    int combined = (left5 << 5) | right5;
    subkeys[0] = permute(combined, P8, 8, 10);

    // LS-2 (2 more shifts)
    left5 = leftShift5(left5, 2);
    right5 = leftShift5(right5, 2);

    // Combine halves and apply P8 -> Subkey2
    combined = (left5 << 5) | right5;
    subkeys[1] = permute(combined, P8, 8, 10);
}


// Function to expand and permute 4-bit half to 8 bits using EP
int expandPermute(int half4) {
    return permute(half4, EP, 8, 4);
}

int gcd(int a, int b){
    if (b == 0) return a;
    return gcd(b, a % b);
}

// Function to apply S-box to 4 bits and get 2 bits output
int sBoxLookup(int input4, int sBox[4][4]) {
    // row: bits 1 & 4, col: bits 2 & 3
    int row = ((input4 & 0b1000) >> 2) | (input4 & 0b0001);
    int col = (input4 & 0b0110) >> 1;
    return sBox[row][col];
}

// Function F: takes right half (4 bits) and subkey (8 bits)
int functionF(int right4, int subkey) {
    int expanded = expandPermute(right4);    // 8 bits
    int xored = expanded ^ subkey;           // XOR with subkey

    // Split into two 4-bit halves for S-box input
    int left4 = (xored >> 4) & 0xF;
    int right4_ = xored & 0xF;

    // S-box lookups
    int s0_out = sBoxLookup(left4, S0);      // 2 bits
    int s1_out = sBoxLookup(right4_, S1);   // 2 bits

    int combined4 = (s0_out << 2) | s1_out;  // 4 bits

    // Apply P4 permutation
    int p4 = permute(combined4, P4, 4, 4);
    return p4;
}

// Round function: L XOR F(R, key)
void roundFunction(int& left4, int right4, int subkey) {
    int f_out = functionF(right4, subkey);
    left4 = left4 ^ f_out;
}

// Initial and inverse permutation functions
int initialPermutation(int input8) {
    return permute(input8, IP, 8, 8);
}

int inverseInitialPermutation(int input8) {
    return permute(input8, IP_inv, 8, 8);
}

// Encryption function
int encrypt(int plaintext8, int* subkeys) {
    int ip = initialPermutation(plaintext8);
    int left4 = (ip >> 4) & 0xF;
    int right4 = ip & 0xF;

    // Round 1 with subkey1
    roundFunction(left4, right4, subkeys[0]);

    // Swap halves
    swap(left4, right4);

    // Round 2 with subkey2
    roundFunction(left4, right4, subkeys[1]);

    // Combine halves
    int combined = (left4 << 4) | right4;

    // Inverse IP
    return inverseInitialPermutation(combined);
}

// Decryption function (same as encryption but subkeys reversed)
int decrypt(int ciphertext8, int* subkeys) {
    int ip = initialPermutation(ciphertext8);
    int left4 = (ip >> 4) & 0xF;
    int right4 = ip & 0xF;

    // Round 1 with subkey2 (reverse order)
    roundFunction(left4, right4, subkeys[1]);

    // Swap halves
    swap(left4, right4);

    // Round 2 with subkey1
    roundFunction(left4, right4, subkeys[0]);

    // Combine halves
    int combined = (left4 << 4) | right4;

    // Inverse IP
    return inverseInitialPermutation(combined);
}

int main() {
    int key = 0b1010000010; // 10-bit key
    int subkeys[2];
    keyGeneration(key, subkeys);

    cout << "Subkey 1: " << bitset<8>(subkeys[0]) << "\n";
    cout << "Subkey 2: " << bitset<8>(subkeys[1]) << "\n";

    int plaintext = 0b11010111; // 8-bit plaintext
    cout << "Plaintext: " << bitset<8>(plaintext) << "\n";

    int ciphertext = encrypt(plaintext, subkeys);
    cout << "Ciphertext: " << bitset<8>(ciphertext) << "\n";

    int decrypted = decrypt(ciphertext, subkeys);
    cout << "Decrypted Text: " << bitset<8>(decrypted) << "\n";

    return 0;
}
