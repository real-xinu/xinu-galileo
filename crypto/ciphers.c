#include <xinu.h>

void none_cipher (void* key, byte *msg, byte *ciphertext) {
  memcpy((char *)ciphertext, (char *) msg, 8);
}

void none_gen(byte* key, void* keyblob) {
    return;
}

struct ciphent ciphtab[3] = { \
  {.enc = none_cipher, .dec = none_cipher, .gen = none_gen, .block_size = 8, .key_size = 0, .keyblob_size = 0, .name = "none"}, \
  {.enc = (CIPHER_FUNCTION)des_enc, .dec = (CIPHER_FUNCTION)des_dec, .gen= (KEYGEN_FUNCTION)des_generatekeys, .block_size = 8, .key_size = 8, .keyblob_size = 16 * 8, .name = "des"}, \
  {.enc = (CIPHER_FUNCTION)tdea_enc, .dec = (CIPHER_FUNCTION)tdea_dec, .gen = (KEYGEN_FUNCTION)tdea_generatekeys, .block_size = 8, .key_size = 24, .keyblob_size = sizeof(struct tdea_keyblob), .name = "tdea"}
};

struct modeent modetab[2] = { \
  {.enc = NULL, .dec = NULL, .name = "ecb"},
  {.enc = cipher_cbc_enc, .dec = cipher_cbc_dec, .name = "cbc"} \
};

