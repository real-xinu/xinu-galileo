#include <xinu.h>

//byte des_key_pc1_byte[8][7] = {
//  {1, 2, 3, 7, 7, 6, 5},
//  {0, 2, 3, 7, 7, 6, 4},
//  {0, 1, 3, 7, 7, 5, 4},
//  {0, 1, 2, 7, 6, 5, 4},
//  {0, 1, 2, 3, 6, 5, 4},
//  {0, 1, 2, 3, 6, 5, 4},
//  {0, 1, 2, 3, 6, 5, 4},
//  {0, 1, 2, 3, 6, 5, 4}
//};
//
//byte des_key_pc1_pos[8][7] = {
//  {6, 5, 4, 0, 4, 5, 6},
//  {0, 6, 5, 1, 5, 6, 0},
//  {1, 0, 6, 2, 6, 0, 1},
//  {2, 1, 0, 3, 0, 1, 2},
//  {3, 2, 1, 0, 1, 2, 3},
//  {4, 3, 2, 1, 2, 3, 4},
//  {5, 4, 3, 2, 3, 4, 5},
//  {6, 5, 4, 3, 4, 5, 6}
//};
//
//void des_key_pc1(uint32* key, byte* pc_key) {
//  uint32 mask, i, j;
//  uint32 keyl, keyr;
//
//  mask = 1 << 31;
//  keyl = *key++;
//  keyr = *key;
//
//  memset(pc_key, 0, 8);
//
//  for (i = 0 ; i < 4 ; i++) {
//    for (j = 0 ; j < 7 ; j++) {
//      if (mask & keyl) {
//        pc_key[des_key_pc1_byte[i][j]] |= 1 << des_key_pc1_pos[i][j];
//      }
//      if (mask & keyr) {
//        pc_key[des_key_pc1_byte[i+4][j]] |= 1 << des_key_pc1_pos[i+4][j];
//      }
//      mask >>= 1;
//    }
//    /* skip 8th bit */
//    mask >>= 1;
//  }
//}

//byte des_key_pc1_word[64] = {
//  0, 0, 0, 1, 1, 1, 1, -1,
//  0, 0, 0, 1, 1, 1, 1, -1,
//  0, 0, 0, 1, 1, 1, 1, -1,
//  0, 0, 0, 1, 1, 1, 1, -1,
//  0, 0, 0, 0, 1, 1, 1, -1,
//  0, 0, 0, 0, 1, 1, 1, -1,
//  0, 0, 0, 0, 1, 1, 1, -1,
//  0, 0, 0, 0, 1, 1, 1, -1
//};

byte des_key_pc1_pos_left[32] = {
  20, 12, 4, 0, 4, 12, 20, -1,
  21, 13, 5, 1, 5, 13, 21, -1,
  22, 14, 6, 2, 6, 14, 22, -1,
  23, 15, 7, 3, 7, 15, 23, -1
};

byte des_key_pc1_pos_right[32] = {
  24, 16, 8, 0, 8, 16, 24, -1,
  25, 17, 9, 1, 9, 17, 25, -1,
  26, 18, 10, 2, 10, 18, 26, -1,
  27, 19, 11, 3, 11, 19, 27, -1
};

byte des_key_pc2C_byte[28] = {
  0, 3, 1, 2, 0, 1, 3, 2, -1,
  1, 0, 2, 3, 0, 1, 3, 0, -1,
  2, 3, 1, -1, 2, 0, -1, 2, 3, 1};

byte des_key_pc2C_pos[28] = {
  1, 0, 5, 2, 0, 2, 4, 0, -1,
  0, 3, 3, 1, 5, 3, 5, 4, -1,
  4, 2, 1, -1, 5, 2, -1, 1, 3, 4};

byte des_key_pc2D_byte[28] = {
  7, 5, 4, 7, 5, 6, -1, 7, 4,
  -1, 6, 5, 4, 7, -1, 6, 5, 7,
  4, 5, 6, 7, 5, 4, 6, -1, 4, 6};

byte des_key_pc2D_pos[28] = {
  1, 5, 3, 0, 1, 1, -1, 2, 2,
  -1, 3, 4, 5, 4, -1, 5, 2, 5,
  1, 0, 4, 3, 3, 4, 0, -1, 0, 2};

byte des_key_shift_n[16] = {
  1, 1, 2, 2, 2, 2, 2, 2,
  1, 2, 2, 2, 2, 2, 2, 1};

void des_key_pc1(byte* key, uint32* C, uint32* D) {
  uint32 mask, i;
  uint32 keyl, keyr;
  uint32 outC, outD;

  mask = 1 << 31;
  keyl = key[0] << 24 | key[1] << 16 | key[2] << 8 | key[3];
  keyr = key[4] << 24 | key[5] << 16 | key[6] << 8 | key[7];
  outC = outD = 0;

  for (i = 0 ; i < 32 ;) {
    if (mask & keyl) {
      outC |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outC |= 1 << des_key_pc1_pos_right[i];
    }
    i++;
    mask >>= 1;
    if (mask & keyl) {
      outC |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outC |= 1 << des_key_pc1_pos_right[i];
    }
    i++;
    mask >>= 1;
    if (mask & keyl) {
      outC |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outC |= 1 << des_key_pc1_pos_right[i];
    }
    i++;
    mask >>= 1;
    if (mask & keyl) {
      outD |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outC |= 1 << des_key_pc1_pos_right[i];
    }
    i++;
    mask >>= 1;
    if (mask & keyl) {
      outD |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outD |= 1 << des_key_pc1_pos_right[i];
    }
    i++;
    mask >>= 1;
    if (mask & keyl) {
      outD |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outD |= 1 << des_key_pc1_pos_right[i];
    }
    i++;
    mask >>= 1;
    if (mask & keyl) {
      outD |= 1 << des_key_pc1_pos_left[i];
    }
    if (mask & keyr) {
      outD |= 1 << des_key_pc1_pos_right[i];
    }
    i += 2;
    mask >>= 2;
  }

  *C = outC;
  *D = outD;
}

void des_key_pc2(uint32 C, uint32 D, byte* key_round) {
  uint32 mask = 1 << 27;
  int32 i;

  memset(key_round, 0, 8);

  for (i = 0 ; i < 28 ; i++) {
    if ((C & mask) && des_key_pc2C_byte[i] != 255) {
      key_round[des_key_pc2C_byte[i]] |= 1 << des_key_pc2C_pos[i];
    }
    if ((D & mask) && des_key_pc2D_byte[i] != 255) {
      key_round[des_key_pc2D_byte[i]] |= 1 << des_key_pc2D_pos[i];
    }
    mask >>= 1;
  }
}

void des_generatekeys(byte* key, byte keys[16][8]) {
  int32 i;
  uint32 C, D;

  des_key_pc1(key, &C, &D);

  for (i = 0 ; i < 16 ; i++) {
    C = des_key_shift(C, des_key_shift_n[i]);
    D = des_key_shift(D, des_key_shift_n[i]);

    des_key_pc2(C, D, keys[i]);
  }
}



