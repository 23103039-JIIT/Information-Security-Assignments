#include <bits/stdc++.h>
using namespace std;

string DES_Encrypt(string plaintext, string key);
string DES_Decrypt(string ciphertext, string key);

// Triple DES Encryption (EDE mode)
string TripleDES_Encrypt(string plaintext, string key1, string key2, string key3) {
    string step1 = DES_Encrypt(plaintext, key1);   // Encrypt with K1
    string step2 = DES_Decrypt(step1, key2);       // Decrypt with K2
    string step3 = DES_Encrypt(step2, key3);       // Encrypt with K3
    return step3; 
}

// Triple DES Decryption (EDE mode)
string TripleDES_Decrypt(string ciphertext, string key1, string key2, string key3) {
    string step1 = DES_Decrypt(ciphertext, key3);  // Decrypt with K3
    string step2 = DES_Encrypt(step1, key2);       // Encrypt with K2
    string step3 = DES_Decrypt(step2, key1);       // Decrypt with K1
    return step3;  
}

int main() {
    string plaintext = "HELLOWORLD";  // Example text
    string key1 = "12345678";  
    string key2 = "23456789";
    string key3 = "34567890";

    cout << "Plaintext: " << plaintext << endl;

    string cipher = TripleDES_Encrypt(plaintext, key1, key2, key3);
    cout << "Encrypted (3DES): " << cipher << endl;

    string decrypted = TripleDES_Decrypt(cipher, key1, key2, key3);
    cout << "Decrypted (3DES): " << decrypted << endl;

    return 0;
}
