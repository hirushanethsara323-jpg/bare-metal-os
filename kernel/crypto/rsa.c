/**
 * Nothing OS - RSA Asymmetric Public-Key Cryptography Subsystem
 * 
 * Implements modular exponentiation, RSA encryption, decryption, and digital signature
 * verification for kernel package security and secure communications.
 */

#include "../include/rsa.h"
#include "../include/serial.h"

/**
 * Perform pure 32-bit modular exponentiation: (base^exp) % mod
 */
static uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t mod) {
    uint32_t res = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            res = (res * base) % mod;
        }
        exp >>= 1;
        base = (base * base) % mod;
    }
    return res;
}

void rsa_init_keys(rsa_key_t* key) {
    if (key == NULL) return;
    /* Prime p = 61, q = 53 -> n = 3233, e = 17, d = 2753 */
    key->n = 3233;
    key->e = 17;
    key->d = 2753;
    klog(KLOG_INFO, "RSA Asymmetric Key Pair Initialized (N=3233, e=17, d=2753).");
}

uint32_t rsa_encrypt(uint32_t msg, uint32_t e, uint32_t n) {
    return mod_exp(msg, e, n);
}

uint32_t rsa_decrypt(uint32_t cipher, uint32_t d, uint32_t n) {
    return mod_exp(cipher, d, n);
}

bool rsa_verify_signature(uint32_t hash, uint32_t signature, uint32_t e, uint32_t n) {
    uint32_t decrypted_hash = rsa_encrypt(signature, e, n);
    return (decrypted_hash == hash);
}
