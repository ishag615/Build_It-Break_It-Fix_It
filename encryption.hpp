#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP

#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <cstring>

using namespace std;

class AESEncryption {
private:
    static const int KEY_SIZE = 32;  // 256 bits
    static const int IV_SIZE = 16;   // 128 bits for AES block
    
    unsigned char key[KEY_SIZE];
    
    // Derive key from token using SHA-256
    void deriveKey(const string& token);
    
public:
    AESEncryption(const string& token);
    
    // Encrypt data and return base64-like hex string
    string encrypt(const string& plaintext);
    
    // Decrypt hex string back to plaintext
    string decrypt(const string& ciphertext);
    
    // Helper to convert bytes to hex string
    static string toHex(const unsigned char* data, int len);
    
    // Helper to convert hex string to bytes
    static vector<unsigned char> fromHex(const string& hex);
};

#endif // ENCRYPTION_HPP
