#include <xinu.h>

void des_enc(byte keyblob[16][8], byte* block, byte* ciphertext) {

  uint32 left, right, tmp;
  int32 i;

  /* IP */
  fast_des_ip(block, &left, &right);

  /* round 1 */
  for (i = 0 ; i < 16 ; i++) {
    tmp = right;
    right = left ^ fast_des_funcf(right, keyblob[i]);
    left = tmp;
  }

  /* IP^-1*/
  fast_des_ipinv(right, left, ciphertext);
}

void des_encrypt(byte key[8], byte* block, byte* ciphertext) {
  byte round_keys[16][8];
  des_generatekeys(key, round_keys);
  des_enc(round_keys, block, ciphertext);
}

void des_dec(byte keyblob[16][8], byte* block, byte* ciphertext) {

  uint32 left, right, tmp;
  int32 i;

  /* IP */
  des_ip(block, &left, &right);

  /* round 1 */
  for (i = 15 ; i >= 0 ; i--) {
    tmp = right;
    right = left ^ fast_des_funcf(right, keyblob[i]);
    left = tmp;
  }

  /* IP^-1*/
  des_ipinv(right, left, ciphertext);
}

void des_decrypt(byte key[8], byte* block, byte* ciphertext) {
  byte round_keys[16][8];
  des_generatekeys(key, round_keys);
  des_dec(round_keys, block, ciphertext);
}
