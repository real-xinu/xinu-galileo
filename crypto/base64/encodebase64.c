#include <xinu.h>

static char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int32 base64_encode(byte* src, int32 length, byte* dst) {
    int32 i, l;

    i = 0;
    l = 0;
    while (i + 2 < length) {
        *dst++ = alphabet[(*src >> 2) & 0x3F];
        *dst++ = alphabet[((*src & 0x3) << 4) | ((*(src + 1) >> 4) & 0xF)];
        src++;
        *dst++ = alphabet[((*src & 0xF) << 2) | ((*(src + 1) >> 6) & 0x3)];
        src++;
        *dst++ = alphabet[*src++ & 0x3F];
        i += 3;
        l += 4;
    }

    switch (length - i) {
        case 0:
            return l;
        case 1:
            *dst++ = alphabet[(*src >> 2) & 0x3F];
            *dst++ = alphabet[(*src & 0x3) << 4];
            *dst++ = BASE64_PADDING;
            *dst++ = BASE64_PADDING;
            return l + 4;
        case 2:
            *dst++ = alphabet[(*src >> 2) & 0x3F];
            *dst++ = alphabet[((*src & 0x3) << 4) | ((*(src + 1) >> 4) & 0xF)];
            *dst++ = alphabet[(*(src + 1) & 0xF) << 2];
            *dst++ = BASE64_PADDING;
            return l + 4;
        default:
            // Nope
            return SYSERR;
    }
}
