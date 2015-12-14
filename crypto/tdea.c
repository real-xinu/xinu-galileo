#include <xinu.h>

void tdea_generatekeys(byte* key, struct tdea_keyblob* key_b) {

  /* key 1 */
  des_generatekeys(key, key_b->key_1);

  /* key 2 */
  des_generatekeys(key + 8, key_b->key_2);

  /* key 3 */
  des_generatekeys(key + 16, key_b->key_3);

}

void tdea_enc(struct tdea_keyblob* key, byte* block, byte* ciphertext) {
  des_enc(key->key_1, block, ciphertext);
  des_dec(key->key_2, ciphertext, ciphertext);
  des_enc(key->key_3, ciphertext, ciphertext);
}

void tdea_dec(struct tdea_keyblob* key, byte* block, byte* ciphertext) {
  des_dec(key->key_3, block, ciphertext);
  des_enc(key->key_2, ciphertext, ciphertext);
  des_dec(key->key_1, ciphertext, ciphertext);
}

void tdea_encrypt(byte key[24], byte* block, byte* ciphertext) {
  des_encrypt(key, block, ciphertext);
  des_decrypt(key + 8, ciphertext, ciphertext);
  des_encrypt(key + 16, ciphertext, ciphertext);
}

void tdea_decrypt(byte key[24], byte* block, byte* ciphertext) {
  des_decrypt(key + 16, block, ciphertext);
  des_encrypt(key + 8, ciphertext, ciphertext);
  des_decrypt(key, ciphertext, ciphertext);
}
