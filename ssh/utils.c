#include <xinu.h>
#define ISLETTER(x) ((x) > 0x1f && (x) < 0x7f)

uint32 strcopy(byte* dst, const char* src) {
    register int i;

    for (i = 0; ((*dst++) = (*src++)) != '\0'; i++) {
    }

    return i;
}

void xdump_binary_packet(struct ssh_binary_packet *pck, bool8 enc, int32 len) {
  int32 i, j;

  if (!enc) {
    kprintf("Packet length: %u, padding length %u\n", ntohl(pck->packet_length), pck->padding_length);
    len = ntohl(pck->packet_length) - 1;
  } else {
    kprintf("Enc pck len: %08x, enc padding length %02x\n", ntohl(pck->packet_length), pck->padding_length);
    len -= 5;
  }

  kprintf("Payload + padding:\n");
  for (i = 0 ; i < len ; i += 16) {

    for (j = i ; j < i + 16 ; ++j)
      if (j < len)
        kprintf("%02x ", pck->buff[j]);
      else
        kprintf("   ");

    kprintf(" ");
    for (j = i ; j < i + 16 ; ++j)
      if (j < len) {
        if (ISLETTER(pck->buff[j]))
          kprintf("%c", pck->buff[j]);
        else
          kprintf(".");
      }

    kprintf("\n");
  }
}

void xdump_array(byte *tab, int32 len) {
  int32 i, j;

  for (i = 0 ; i < len ; i += 16) {

    for (j = i ; j < i + 16 ; ++j)
      if (j < len)
        kprintf("%02x ", tab[j]);
      else
        kprintf("   ");

    kprintf(" ");
    for (j = i ; j < i + 16 ; ++j)
      if (j < len) {
        if (ISLETTER(tab[j]))
          kprintf("%c", tab[j]);
        else
          kprintf(".");
      }

    kprintf("\n");
  }

}

void bdump_array(byte *tab, int32 len) {
  int32 i, j, k;

  for (i = 0 ; i < len ; i += 4) {

    for (j = i ; j < i + 4 ; ++j)
      if (j < len) {
        for (k = 0x80 ; k > 0 ; k >>= 1) {
          kprintf("%d", (tab[j] & k) != 0 ? 1 : 0);
        }
        kprintf(" ");
      } else
        kprintf("         ");

    kprintf(" ");
    for (j = i ; j < i + 4 ; ++j)
      if (j < len) {
        if (ISLETTER(tab[j]))
          kprintf("%c", tab[j]);
        else
          kprintf(".");
      }

    kprintf("\n");
  }

}

void bdump_int(uint32 val) {
  uint32 i, j;

  for (j = 0x80000000, i = 0 ; j > 0 ; j >>= 1, i++) {
      kprintf("%d", (val & j) != 0 ? 1 : 0);
      if (i % 8 == 7) {
        kprintf(" ");
      }
  }
  kprintf("\n");
}
