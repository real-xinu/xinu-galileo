#include <xinu.h>

/* Specification found in ITU-T X.690 document */

int32 asn1read_identifier(uint32* idp, byte* classp, byte* pcp, byte* src) {

	int32 i;
	uint32 tmp;
	*classp = *src & 0xc0;
	*pcp = *src & 0x20;

	if ((*src & 0x1F) != 0x1F) {
		*idp = *src & 0x1F;
		return 1;
	}
	src++;

	tmp = 0;
	i = 1;
	do {
		i++;
		tmp = (tmp << 7) | (*src & 0x7F);
	} while (*src++ & 0x80);

	*idp = tmp;
	return i;
}

int32 asn1read_length(uint32* lengthp, byte* src) {

	byte i, l;
	uint32 tmp;

	if (*src == 0x80) {
		*lengthp = -1;
		return 1;
	}

	if ((*src & 0x80) == 0) {
		*lengthp = *src & 0xFF;
		return 1;
	}

	i = *src++ & 0x3F;
	l = i + 1;
	tmp = 0;
	while (i--) {
		tmp = (tmp << 8) | (*src++ & 0xFF);
	}

	*lengthp = tmp;
	return l;
}

int32 asn1is_end_content(byte* src) {
	if (*src++ == 0 && *src == 0) {
		return 2;
	}

	return 0;
}


