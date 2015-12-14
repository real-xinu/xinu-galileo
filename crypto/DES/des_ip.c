#include <xinu.h>

#define BYTE(x, n) ((byte)(((x) >> (8 * (n))) & 0xFF))

/* NIST SP 800-67 Revison 1 - page 5 */
//byte des_ip_tab[64] = {
//  58, 50, 42, 34, 26, 18, 10, 2,
//  60, 52, 44, 36, 28, 20, 12, 4,
//  62, 54, 46, 38, 30, 22, 14, 6,
//  64, 56, 48, 40, 32, 24, 16, 8,
//  57, 49, 41, 33, 25, 17,  9, 1,
//  59, 51, 43, 35, 27, 19, 11, 3,
//  61, 53, 45, 37, 29, 21, 13, 5,
//  63, 55, 47, 39, 31, 23, 15, 7};

byte des_ip_tab[16] = {
  26, 18, 10, 2,
  28, 20, 12, 4,
  30, 22, 14, 6,
  32, 24, 16, 8};

//byte des_ipinv_tab[64] = {
//  40,  8, 48, 16, 56, 24, 64, 32,
//  39,  7, 47, 15, 55, 23, 63, 31,
//  38,  6, 46, 14, 54, 22, 62, 30,
//  37,  5, 45, 13, 53, 21, 61, 29,
//  36,  4, 44, 12, 52, 20, 60, 28,
//  35,  3, 43, 11, 51, 19, 59, 27,
//  34,  2, 42, 10, 50, 18, 58, 26,
//  33,  1, 41,  9, 49, 17, 57, 25};

byte des_ipinv_tab[32] = {
   8, 16, 24, 32,
   7, 15, 23, 31,
   6, 14, 22, 30,
   5, 13, 21, 29,
   4, 12, 20, 28,
   3, 11, 19, 27,
   2, 10, 18, 26,
   1,  9, 17, 25};

uint32 des_ip_left[8][256];
uint32 des_ip_right[8][256];
uint32 des_ipinv_left[8][256];
uint32 des_ipinv_right[8][256];

/* non efficient implementation of IP and IP^-1*/
//void des_ip(uint32* in, uint32* left, uint32* right) {
//
//  uint32 inl, inr, outl, outr;
//  uint32 i, mask;
//
//  inl = *in++;
//  inr = *in;
//  outl = 0;
//  outr = 0;
//  mask = 1 << 31;
//
//  for (i = 0 ; i < 16 ; i++) {
//    if (mask & inl) {
//      outr |= 1 << (32 - des_ipinv_tab[i]);
//    }
//    if (mask & inr) {
//      outr |= 1 << (32 - des_ipinv_tab[16 + i]);
//    }
//    mask >>= 1;
//    if (mask & inl) {
//      outl |= 1 << (32 - des_ipinv_tab[i]);
//    }
//    if (mask & inr) {
//      outl |= 1 << (32 - des_ipinv_tab[16 + i]);
//    }
//    mask >>= 1;
//  }
//
//  *left = outl;
//  *right = outr;
//}

void des_ip(byte* in, uint32* left, uint32* right) {
  uint32 inl, inr;
  byte outl[4], outr[4];
  uint32 mask, mask1;
  int32 i, j;

  inl = in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
  inr = in[4] << 24 | in[5] << 16 | in[6] << 8 | in[7];
  outl[0] = outl[1] = outl[2] = outl[3] = 0;
  outr[0] = outr[1] = outr[2] = outr[3] = 0;
  mask = 1 << 31;
  mask1 = 1;

  for (i = 0 ; i < 4 ; i++) {
    for (j = 0 ; j < 4 ; j++) {
      if (inl & mask) {
        outr[j] |= mask1;
      }
      if (inr & mask) {
        outr[j] |= mask1 << 4;
      }
      mask >>= 1;
      if (inl & mask) {
        outl[j] |= mask1;
      }
      if (inr & mask) {
        outl[j] |= mask1 << 4;
      }
      mask >>= 1;
    }
    mask1 <<= 1;
  }

  *left = outl[0] << 24 | outl[1] << 16 | outl[2] << 8 | outl[3];
  *right = outr[0] << 24 | outr[1] << 16 | outr[2] << 8 | outr[3];
}

void des_ipinv(uint32 left, uint32 right, byte* out) {
  uint32 mask, mask1;
  int32 i, j;

  memset(out, 0, 8);

  mask = 1 << 31;
  mask1 = 1 << 6;

  for (i = 0 ; i < 4 ; i++) {
    for (j = 7 ; j > -1 ; j--) {
      if (mask & left) {
        out[j] |= mask1;
      }
      if (mask & right) {
        out[j] |= mask1 << 1;
      }
      mask >>= 1;
    }
    mask1 >>= 2;
  }
}

void generate_desip_table() {
  int16 val;
  byte in[8], out[8];
  int32 bytepos;
  uint32 left, right;

  memset(in, 0, 8);

  for (bytepos = 0 ; bytepos < 8 ; bytepos++) {
    for (val = 0 ; val < 0x100 ; val++) {
      in[bytepos] = val;
      des_ip(in, &(des_ip_left[bytepos][val]), &(des_ip_right[bytepos][val]));
    }
    in[bytepos] = 0;
  }

  for (bytepos = 0 ; bytepos < 32 ; bytepos += 8) {
    for (val = 0 ; val < 0x100 ; val++) {
      memset(out, 0, 8);
      right = val << bytepos;
      left = 0;
      des_ipinv(left, right, out);
      des_ipinv_left[4 + bytepos/8][val] = out[0] << 24 | out[1] << 16 | out[2] << 8 | out[3];
      des_ipinv_right[4 +  bytepos/8][val] = out[4] << 24 | out[5] << 16 | out[6] << 8 | out[7];

      memset(out, 0, 8);
      right = 0;
      left = val << bytepos;
      des_ipinv(left, right, out);
      des_ipinv_left[bytepos/8][val] = out[0] << 24 | out[1] << 16 | out[2] << 8 | out[3];
      des_ipinv_right[bytepos/8][val] = out[4] << 24 | out[5] << 16 | out[6] << 8 | out[7];
    }
  }
}

void fast_des_ip(byte* in, uint32* left, uint32* right) {
  uint32 outl, outr;

  outl = outr = 0;

  outl |= des_ip_left[0][*in];
  outr |= des_ip_right[0][*in];

  in++;
  outl |= des_ip_left[1][*in];
  outr |= des_ip_right[1][*in];

  in++;
  outl |= des_ip_left[2][*in];
  outr |= des_ip_right[2][*in];

  in++;
  outl |= des_ip_left[3][*in];
  outr |= des_ip_right[3][*in];

  in++;
  outl |= des_ip_left[4][*in];
  outr |= des_ip_right[4][*in];

  in++;
  outl |= des_ip_left[5][*in];
  outr |= des_ip_right[5][*in];

  in++;
  outl |= des_ip_left[6][*in];
  outr |= des_ip_right[6][*in];

  in++;
  outl |= des_ip_left[7][*in];
  outr |= des_ip_right[7][*in];

  *left = outl;
  *right = outr;
}

void fast_des_ipinv(uint32 left, uint32 right, byte* out) {
  byte val;
  uint32 outl, outr;

  outl = outr = 0;

  val = BYTE(left, 0);
  outl |= des_ipinv_left[0][val];
  outr |= des_ipinv_right[0][val];

  val = BYTE(left, 1);
  outl |= des_ipinv_left[1][val];
  outr |= des_ipinv_right[1][val];

  val = BYTE(left, 2);
  outl |= des_ipinv_left[2][val];
  outr |= des_ipinv_right[2][val];

  val = BYTE(left, 3);
  outl |= des_ipinv_left[3][val];
  outr |= des_ipinv_right[3][val];

  val = BYTE(right, 0);
  outl |= des_ipinv_left[4][val];
  outr |= des_ipinv_right[4][val];

  val = BYTE(right, 1);
  outl |= des_ipinv_left[5][val];
  outr |= des_ipinv_right[5][val];

  val = BYTE(right, 2);
  outl |= des_ipinv_left[6][val];
  outr |= des_ipinv_right[6][val];

  val = BYTE(right, 3);
  outl |= des_ipinv_left[7][val];
  outr |= des_ipinv_right[7][val];

  out[0] = outl >> 24;
  out[1] = (outl >> 16) & 0xFF;
  out[2] = (outl >> 8) & 0xFF;
  out[3] = outl & 0xFF;
  out[4] = outr >> 24;
  out[5] = (outr >> 16) & 0xFF;
  out[6] = (outr >> 8) & 0xFF;
  out[7] = outr & 0xFF;
}
