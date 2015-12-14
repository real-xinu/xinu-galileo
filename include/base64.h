#ifndef BASE64_H
#define BASE64_H

#define BASE64_PADDING ('=')
#define BASE64_ENCODED_SIZE(l) (((l) + 2)/3 * 4)
#define BASE64_DECODED_SIZE(l) ((l) / 4 * 3)

int32 base64_decode(byte* src, int32 length, byte* dst);
int32 base64_encode(byte* src, int32 length, byte* dst);

void launch_base64();

#endif
