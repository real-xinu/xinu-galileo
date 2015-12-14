#include <xinu.h>

void launch_base64() {

    char in1[] = "\x14\xfb\x9c\x03\xd9\x7e";
    char in2[] = "\x14\xfb\x9c\x03\xd9";
    char in3[] = "\x14\xfb\x9c\x03";
    char output[32];
    char ex1[] = "FPucA9l+";
    char ex2[] = "FPucA9k=";
    char ex3[] = "FPucAw==";

    kprintf("Launch base64 tests\n");

    memset(output, 0, 32);
    if (BASE64_ENCODED_SIZE(strnlen(in1, 100)) != base64_encode((byte *)in1, strnlen(in1, 100), (byte *)output) ||
            strncmp(output, ex1, 32) != 0) {
        kprintf("\ttest 1.e: FAIL ==> %s <> %s\n", output, ex1);
        return;
    }

    memset(output, 0, 32);
    if (BASE64_DECODED_SIZE(strnlen(ex1, 100)) < base64_decode((byte *)ex1, strnlen(ex1, 100), (byte *)output) ||
            strncmp(output, in1, 32) != 0) {
        kprintf("\ttest 1.d: FAIL\n");
        xdump_array((byte *)in1, 7);
        xdump_array((byte *)output, 7);
        return;
    }

    memset(output, 0, 32);
    if (BASE64_ENCODED_SIZE(strnlen(in2, 100)) != base64_encode((byte *)in2, strnlen(in2, 100), (byte *)output) ||
            strncmp(output, ex2, 32) != 0) {
        kprintf("\ttest 2.e: FAIL ==> %s <> %s\n", output, ex2);
        return;
    }

    memset(output, 0, 32);
    if (BASE64_DECODED_SIZE(strnlen(ex2, 100)) < base64_decode((byte *)ex2, strnlen(ex2, 100), (byte *)output) ||
            strncmp(output, in2, 32) != 0) {
        kprintf("\ttest 2.d: FAIL\n");
        xdump_array((byte *)in2, 6);
        xdump_array((byte *)output, 6);
        return;
    }

    memset(output, 0, 32);
    if (BASE64_ENCODED_SIZE(strnlen(in3, 100)) != base64_encode((byte *)in3, strnlen(in3, 100), (byte *)output) ||
            strncmp(output, ex3, 32) != 0) {
        kprintf("\ttest 3.e: FAIL ==> %s <> %s\n", output, ex3);
        return;
    }

    memset(output, 0, 32);
    if (BASE64_DECODED_SIZE(strnlen(ex3, 100)) < base64_decode((byte *)ex3, strnlen(ex3, 100), (byte *)output) ||
            strncmp(output, in3, 32) != 0) {
        kprintf("\ttest 3.d: FAIL\n");
        xdump_array((byte *)in3, 5);
        xdump_array((byte *)output, 5);
        return;
    }

    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    byte sol[] = "\x00\x10\x83\x10\x51\x87\x20\x92\x8b\x30\xd3\x8f\x41\x14\x93\x51\x55\x97\x61\x96\x9b\x71\xd7\x9f\x82\x18\xa3\x92\x59\xa7\xa2\x9a\xab\xb2\xdb\xaf\xc3\x1c\xb3\xd3\x5d\xb7\xe3\x9e\xbb\xf3\xdf\xbf";

    byte outputal[256];
    memset(outputal, 0, 256);
    char res[65];
    res[64] = '\0';

    int32 length;
    if ((length = base64_decode((byte *)alphabet, 64, (byte *)outputal)) > BASE64_DECODED_SIZE(64)) {
        kprintf("\ttest 4.d.1: FAIL\n");
        return;
    }

    int32 i = 0;
    while (i < length && outputal[i] == sol[i]) {
        i++;
    }
    if (i < length) {
        xdump_array((byte *)outputal, length);
        kprintf("\ttest 4.d.2: FAIL ==> %d <> %d\n", sol[i], outputal[i]);
        return;
    }

    if (64 != base64_encode((byte *)outputal, length, (byte *)res) ||
            strncmp(res, alphabet, 64) != 0) {
        kprintf("\ttest 4.e: FAIL ==> %s <> %s\n", res, alphabet);
        return;
    }

    kprintf("\ttests: OK\n");
}
