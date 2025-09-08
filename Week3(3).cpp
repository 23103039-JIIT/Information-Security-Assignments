// des.cpp
#include <bits/stdc++.h>
using namespace std;
using u64 = uint64_t;
using u32 = uint32_t;
using u8  = uint8_t;

// Helper: get bit at position pos (1..n) where pos=1 is MSB for a 64-bit word
inline int get_bit(u64 v, int pos, int width=64) {
    // pos: 1 = MSB (most-significant), width = total bits of v (default 64)
    int idx = width - pos; // zero-based from LSB = 0
    return (v >> idx) & 1ULL;
}

// Helper: set bit at position pos (1..out_len), pos=1 -> MSB in out_len sized result
inline void set_bit(u64 &out, int pos, int val, int out_len) {
    int idx = out_len - pos;
    if (val) out |= (1ULL << idx);
    else out &= ~(1ULL << idx);
}

// Generic permutation: table entries are 1-based positions in input (MSB=1)
u64 permute(u64 input, const vector<int>& table, int input_width, int out_width) {
    u64 out = 0;
    for (int i = 0; i < (int)table.size(); ++i) {
        int in_pos = table[i]; // 1-based
        int bit = get_bit(input, in_pos, input_width);
        set_bit(out, i + 1, bit, out_width);
    }
    return out;
}

// DES Tables (standard)
const vector<int> PC1 = {
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
}; // 64 -> 56 (drops parity bits)

const vector<int> PC2 = {
    14,17,11,24,1,5,
    3,28,15,6,21,10,
    23,19,12,4,26,8,
    16,7,27,20,13,2,
    41,52,31,37,47,55,
    30,40,51,45,33,48,
    44,49,39,56,34,53,
    46,42,50,36,29,32
}; // 56 -> 48

const vector<int> SHIFTS = {
    1,1,2,2,2,2,2,2,
    1,2,2,2,2,2,2,1
};

const vector<int> IP = {
    58,50,42,34,26,18,10,2,
    60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,
    64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1,
    59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,
    63,55,47,39,31,23,15,7
};

const vector<int> IP_INV = {
    40,8,48,16,56,24,64,32,
    39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,
    37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,
    35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,
    33,1,41,9,49,17,57,25
};

const vector<int> E = {
    32,1,2,3,4,5,
    4,5,6,7,8,9,
    8,9,10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,1
};

const vector<int> P = {
    16,7,20,21,
    29,12,28,17,
    1,15,23,26,
    5,18,31,10,
    2,8,24,14,
    32,27,3,9,
    19,13,30,6,
    22,11,4,25
};

// S-boxes: sbox[box][row][col] (box 0..7)
const int SBOX[8][4][16] = {
    { // S1
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    { // S2
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    { // S3
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    { // S4
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    { // S5
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    { // S6
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    { // S7
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    { // S8
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

// Expand 32-bit R to 48 bits using E
u64 expand32to48(u32 r) {
    u64 in = (u64)r;
    return permute(in, E, 32, 48);
}

// Apply S-box substitution to 48-bit input => 32-bit output
u32 sbox_substitution(u64 in48) {
    u32 out32 = 0;
    for (int s = 0; s < 8; ++s) {
        // Grab 6 bits for this S-box (positions 6*s+1 .. 6*s+6), left->right
        int start_pos = 6 * s + 1;
        int six = 0;
        for (int b = 0; b < 6; ++b) {
            six <<= 1;
            six |= get_bit(in48, start_pos + b, 48);
        }
        int row = ((six & 0x20) >> 4) | (six & 0x1); // bits 6 and 1 -> row
        int col = (six >> 1) & 0xF;                 // bits 2..5 -> column
        int val = SBOX[s][row][col];
        out32 = (out32 << 4) | (val & 0xF);
    }
    return out32;
}

// Feistel function: takes 32-bit R and 48-bit subkey -> 32-bit output
u32 feistel(u32 r, u64 subkey48) {
    u64 expanded = expand32to48(r);       // 48 bits
    u64 xored = expanded ^ subkey48;
    u32 sboxed = sbox_substitution(xored);
    u32 permuted = (u32)permute((u64)sboxed, P, 32, 32);
    return permuted;
}

// Key schedule: from 64-bit key -> 16 subkeys (each 48-bit)
vector<u64> generate_subkeys(u64 key64) {
    // Step 1: PC-1 (64 -> 56)
    u64 perm56 = permute(key64, PC1, 64, 56);

    // Split into C and D (each 28 bits). We'll store as 28-bit values in lower bits.
    u64 C = (perm56 >> 28) & 0x0FFFFFFFULL;
    u64 D = perm56 & 0x0FFFFFFFULL;

    vector<u64> subkeys(16);
    for (int round = 0; round < 16; ++round) {
        int shift = SHIFTS[round];
        // left rotate 28-bit halves
        C = ((C << shift) | (C >> (28 - shift))) & 0x0FFFFFFFULL;
        D = ((D << shift) | (D >> (28 - shift))) & 0x0FFFFFFFULL;

        u64 combined56 = (C << 28) | D; // 56 bits
        // PC-2 => 48 bits
        u64 k48 = permute(combined56, PC2, 56, 48);
        subkeys[round] = k48;
    }
    return subkeys;
}

// 64-bit block encryption
u64 des_block_encrypt(u64 block, const vector<u64>& subkeys) {
    // Initial Permutation
    u64 ip = permute(block, IP, 64, 64);
    u32 L = (u32)(ip >> 32);
    u32 R = (u32)(ip & 0xFFFFFFFFULL);

    for (int round = 0; round < 16; ++round) {
        u32 nextL = R;
        u32 f = feistel(R, subkeys[round]);
        u32 nextR = L ^ f;
        L = nextL;
        R = nextR;
    }
    // After 16 rounds, combine R and L (note swap)
    u64 preoutput = ((u64)R << 32) | (u64)L;
    // Final permutation
    u64 cipher = permute(preoutput, IP_INV, 64, 64);
    return cipher;
}

// 64-bit block decryption (subkeys in reverse)
u64 des_block_decrypt(u64 block, const vector<u64>& subkeys) {
    // Initial Permutation
    u64 ip = permute(block, IP, 64, 64);
    u32 L = (u32)(ip >> 32);
    u32 R = (u32)(ip & 0xFFFFFFFFULL);

    for (int round = 15; round >= 0; --round) {
        u32 nextL = R;
        u32 f = feistel(R, subkeys[round]);
        u32 nextR = L ^ f;
        L = nextL;
        R = nextR;
    }
    u64 preoutput = ((u64)R << 32) | (u64)L;
    u64 plain = permute(preoutput, IP_INV, 64, 64);
    return plain;
}

// Utility: print hex padded
string u64_to_hex(u64 v) {
    std::ostringstream oss;
    oss<<std::hex<<std::uppercase<<std::setfill('0')<<std::setw(16)<<v;
    return oss.str();
}

int main() {
    // Example test vector
    u64 key = 0x133457799BBCDFF1ULL;      // 64-bit key with parity bits
    u64 plaintext = 0x0123456789ABCDEFULL;

    // Generate subkeys
    auto subkeys = generate_subkeys(key);

    // Encrypt
    u64 ciphertext = des_block_encrypt(plaintext, subkeys);

    // Decrypt
    u64 decrypted = des_block_decrypt(ciphertext, subkeys);

    cout << "Key       : 0x" << u64_to_hex(key) << "\n";
    cout << "Plaintext : 0x" << u64_to_hex(plaintext) << "\n";
    cout << "Ciphertext: 0x" << u64_to_hex(ciphertext) << "\n";
    cout << "Decrypted : 0x" << u64_to_hex(decrypted) << "\n";

    // Check expected ciphertext for test vector
    // Expected cipher for this key/plaintext: 0x85E813540F0AB405
    cout << "\nExpected Ciphertext: 0x85E813540F0AB405\n";
    if (ciphertext == 0x85E813540F0AB405ULL) {
        cout << "Test vector MATCHES expected ciphertext.\n";
    } else {
        cout << "Test vector DOES NOT match expected ciphertext.\n";
    }

    return 0;
}
