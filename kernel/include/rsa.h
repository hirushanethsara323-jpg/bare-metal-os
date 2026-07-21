#ifndef RSA_H
#define RSA_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* RSA Key Pair Structure */
typedef struct {
    uint32_t n; /* Modulus */
    uint32_t e; /* Public Exponent */
    uint32_t d; /* Private Exponent */
} rsa_key_t;

/* Function Declarations */
void rsa_init_keys(rsa_key_t* key);
uint32_t rsa_encrypt(uint32_t msg, uint32_t e, uint32_t n);
uint32_t rsa_decrypt(uint32_t cipher, uint32_t d, uint32_t n);
bool rsa_verify_signature(uint32_t hash, uint32_t signature, uint32_t e, uint32_t n);

#endif /* RSA_H */
