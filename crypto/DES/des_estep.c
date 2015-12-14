#include <xinu.h>

void des_estep(uint32 val, byte* key, byte* out) {
  *out++ = *key++ ^ (((val & 1) << 5) | (val >> 27));
  *out++ = *key++ ^ ((val >> 23) & 0x3F);
  *out++ = *key++ ^ ((val >> 19) & 0x3F);
  *out++ = *key++ ^ ((val >> 15) & 0x3F);
  *out++ = *key++ ^ ((val >> 11) & 0x3F);
  *out++ = *key++ ^ ((val >> 7) & 0x3F);
  *out++ = *key++ ^ ((val >> 3) & 0x3F);
  *out = *key ^ (((val << 1) & 0x3F) | (val >> 31));
}
