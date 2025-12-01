#include "encryption.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

AESEncryption::AESEncryption(const string& token) {
    deriveKey(token);
}

void AESEncryption::deriveKey(const string& token) {
    // Use SHA-256 to derive a 256-bit key from the token
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)token.c_str(), token.length(), hash);
    memcpy(key, hash, KEY_SIZE);
}

string AESEncryption::encrypt(const string& plaintext) {
    // Generate random IV
    unsigned char iv[IV_SIZE];
    if (!RAND_bytes(iv, IV_SIZE)) {
        return "";
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return "";
    
    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    // Allocate output buffer
    int plaintext_len = plaintext.length();
    int cipher_len = plaintext_len + AES_BLOCK_SIZE;
    unsigned char* ciphertext = new unsigned char[cipher_len];
    
    int len;
    // Encrypt
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, 
                         (unsigned char*)plaintext.c_str(), plaintext_len) != 1) {
        delete[] ciphertext;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    int ciphertext_len = len;
    
    // Finalize encryption
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
        delete[] ciphertext;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    // Prepend IV to ciphertext and convert to hex
    string result = toHex(iv, IV_SIZE) + toHex(ciphertext, ciphertext_len);
    
    delete[] ciphertext;
    return result;
}

string AESEncryption::decrypt(const string& ciphertext_hex) {
    if (ciphertext_hex.length() < IV_SIZE * 2) {
        return "";
    }
    
    // Extract IV from first 32 hex chars (16 bytes)
    string iv_hex = ciphertext_hex.substr(0, IV_SIZE * 2);
    string cipher_hex = ciphertext_hex.substr(IV_SIZE * 2);
    
    vector<unsigned char> iv_vec = fromHex(iv_hex);
    vector<unsigned char> cipher_vec = fromHex(cipher_hex);
    
    if (iv_vec.size() != IV_SIZE || cipher_vec.empty()) {
        return "";
    }
    
    unsigned char* iv = &iv_vec[0];
    unsigned char* ciphertext = &cipher_vec[0];
    int ciphertext_len = cipher_vec.size();
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return "";
    
    // Initialize decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    
    // Allocate output buffer
    unsigned char* plaintext = new unsigned char[ciphertext_len + AES_BLOCK_SIZE];
    int len;
    int plaintext_len;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1) {
        delete[] plaintext;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len = len;
    
    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
        delete[] plaintext;
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    string result((char*)plaintext, plaintext_len);
    delete[] plaintext;
    
    return result;
}

string AESEncryption::toHex(const unsigned char* data, int len) {
    stringstream ss;
    ss << hex << setfill('0');
    for (int i = 0; i < len; i++) {
        ss << setw(2) << (int)data[i];
    }
    return ss.str();
}

vector<unsigned char> AESEncryption::fromHex(const string& hex) {
    vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}
