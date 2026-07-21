#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define AES256_KEY_SIZE   32
#define AES256_BLOCK_SIZE 16

/* Function Declarations */
void aes256_encrypt_block(const uint8_t key[32], const uint8_t plain[16], uint8_t cipher[16]);
void aes256_decrypt_block(const uint8_t key[32], const uint8_t cipher[16], uint8_t plain[16]);

#endif /* AES_H */
