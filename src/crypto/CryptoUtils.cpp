
#include "CryptoUtils.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

namespace CryptoUtils {

static constexpr int SALT_SIZE = 16;
static constexpr int IV_SIZE = 16;
static constexpr int KEY_SIZE = 32; // AES-256
static constexpr int PBKDF2_ITERS = 100000;

static void handleErrors() {
    ERR_print_errors_fp(stderr);
}

// Derive key from password+salt via PBKDF2-HMAC-SHA256
static bool deriveKey(const string &password, const unsigned char *salt, unsigned char *key_out) {
    // OpenSSL PKCS5_PBKDF2_HMAC
    if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                            salt, SALT_SIZE,
                            PBKDF2_ITERS,
                            EVP_sha256(),
                            KEY_SIZE, key_out)) {
        return false;
    }
    return true;
}

bool encryptFile(const string &inPath, const string &outPath, const string &password) {
    ifstream in(inPath, ios::binary);
    if (!in) return false;
    ofstream out(outPath, ios::binary);
    if (!out) return false;

    ERR_clear_error();
    unsigned char salt[SALT_SIZE];
    if (!RAND_bytes(salt, SALT_SIZE)) { handleErrors(); return false; }
    unsigned char key[KEY_SIZE];
    if (!deriveKey(password, salt, key)) { return false; }

    unsigned char iv[IV_SIZE];
    if (!RAND_bytes(iv, IV_SIZE)) { handleErrors(); return false; }

    // Write salt and iv to output
    out.write(reinterpret_cast<char*>(salt), SALT_SIZE);
    out.write(reinterpret_cast<char*>(iv), IV_SIZE);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) { handleErrors(); return false; }
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        handleErrors(); EVP_CIPHER_CTX_free(ctx); return false;
    }

    const size_t BUF_SIZE = 4096;
    vector<unsigned char> inbuf(BUF_SIZE), outbuf(BUF_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int outlen;

    while (in) {
        in.read(reinterpret_cast<char*>(inbuf.data()), BUF_SIZE);
        streamsize len = in.gcount();
        if (len > 0) {
            if (1 != EVP_EncryptUpdate(ctx, outbuf.data(), &outlen, inbuf.data(), len)) {
                handleErrors(); EVP_CIPHER_CTX_free(ctx); return false;
            }
            out.write(reinterpret_cast<char*>(outbuf.data()), outlen);
        }
    }
    if (1 != EVP_EncryptFinal_ex(ctx, outbuf.data(), &outlen)) {
        handleErrors(); EVP_CIPHER_CTX_free(ctx); return false;
    }
    out.write(reinterpret_cast<char*>(outbuf.data()), outlen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decryptFile(const string &inPath, const string &outPath, const string &password) {
    ifstream in(inPath, ios::binary);
    if (!in) return false;
    // Read salt and iv
    unsigned char salt[SALT_SIZE], iv[IV_SIZE];
    in.read(reinterpret_cast<char*>(salt), SALT_SIZE);
    if (in.gcount() != SALT_SIZE) return false;
    in.read(reinterpret_cast<char*>(iv), IV_SIZE);
    if (in.gcount() != IV_SIZE) return false;

    unsigned char key[KEY_SIZE];
    if (!deriveKey(password, salt, key)) return false;

    ofstream out(outPath, ios::binary);
    if (!out) return false;

    ERR_clear_error();
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) { handleErrors(); return false; }
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        handleErrors(); EVP_CIPHER_CTX_free(ctx); return false;
    }

    const size_t BUF_SIZE = 4096;
    vector<unsigned char> inbuf(BUF_SIZE), outbuf(BUF_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int outlen;
    while (in) {
        in.read(reinterpret_cast<char*>(inbuf.data()), BUF_SIZE);
        streamsize len = in.gcount();
        if (len > 0) {
            if (1 != EVP_DecryptUpdate(ctx, outbuf.data(), &outlen, inbuf.data(), len)) {
                handleErrors(); EVP_CIPHER_CTX_free(ctx); return false;
            }
            out.write(reinterpret_cast<char*>(outbuf.data()), outlen);
        }
    }
    if (1 != EVP_DecryptFinal_ex(ctx, outbuf.data(), &outlen)) {
        handleErrors(); EVP_CIPHER_CTX_free(ctx); return false;
    }
    out.write(reinterpret_cast<char*>(outbuf.data()), outlen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

} // namespace CryptoUtils
