#include <xinu.h>

byte des_perm_tab[][4] = {
  {23, 15,  9,  1}, {19,  4, 30, 14},
  { 8, 16,  2, 26},  {6, 12, 22, 31},
  {24, 18,  7, 29}, {28,  3, 21, 13},
  {0, 20, 10, 25}, {27,  5, 17, 11}};

uint32 des_sboxes_perm_table[8][64];

uint32 des_perm(byte* in) {
  int32 i, j;
  uint32 mask;
  uint32 out = 0;

  for (i = 0 ; i < 8 ; i++) {
    mask = 1 << 3;
    for (j = 0 ; j < 4 ; j++) {
      if (mask & in[i]) {
        out |= 1 << des_perm_tab[i][j];
      }
      mask >>= 1;
    }
  }

  return out;
}

void generate_desperm_table() {
  int16 val;
  byte in[8];
  int32 sbox;

  memset(in, 0 , 8);
  for (sbox = 0 ; sbox < 8 ; sbox++) {
    for (val = 0 ; val < 0x40 ; val++) {
      in[sbox] = s_boxes[sbox][((val >> 4) & 0x2) | (val & 0x1)][(val >> 1) & 0xF];
      des_sboxes_perm_table[sbox][val] = des_perm(in);
    }
    in[sbox] = 0;
  }
}

uint32 des_funcf(uint32 R, byte key[8]) {
  int32 sbox;
  byte interm[8];
  byte* wt = interm;

  /* e-step + xor with key */
  fast_des_estep(R, key, wt);

  /* s - boxes */
  for (sbox = 0 ; sbox < 8 ; sbox++) {
    interm[sbox] = s_boxes[sbox][((interm[sbox] >> 4) & 0x2) | (interm[sbox]  & 0x1)][(interm[sbox] >> 1) & 0xF];
  }

  /* final perm */
  return des_perm(interm);
}

uint32 fast_des_funcf(uint32 R, byte key[8]) {
  uint32 out;
  byte interm[8];
  byte* wt = interm;

  /* e-step + xor with key */
  fast_des_estep(R, key, wt);

  /* s - boxes */
  out = 0;
  out |= des_sboxes_perm_table[0][interm[0]];
  out |= des_sboxes_perm_table[1][interm[1]];
  out |= des_sboxes_perm_table[2][interm[2]];
  out |= des_sboxes_perm_table[3][interm[3]];
  out |= des_sboxes_perm_table[4][interm[4]];
  out |= des_sboxes_perm_table[5][interm[5]];
  out |= des_sboxes_perm_table[6][interm[6]];
  out |= des_sboxes_perm_table[7][interm[7]];

  return out;
}
