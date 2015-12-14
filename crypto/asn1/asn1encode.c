#include <xinu.h>

/* Specification found in ITU-T X.690 document */

int32 asn1write_identifier(byte* dst, uint32 id, byte class, byte pc) {

	byte tmp;
	uint32 mask;
	int32 bitl;

	/* construct leading byte */
	tmp = 0;
	tmp |= (class & 0xc0);
	tmp |= pc & 0x20;

	if (id <= 30) {
		tmp |= id;
		*dst = tmp;
		return 1;
	}

	tmp |= 0x1F;
	*dst++ = tmp;

	/* compute the number of subsequent bytes */
	mask = 1 << 31;
	bitl = 31;
	while (!(id & mask)) {
		mask >>= 1;
		bitl--;
	}

	bitl = bitl / 7;
	dst += bitl;

	/* write last subsequent byte */
	tmp = 0;
	tmp |= id & 0x7F;
	id >>= 7;
	*dst-- = tmp;

	/* write other subsequent byte */
	while (id) {
		tmp = 0x80;
		tmp |= id & 0x7F;
		id >>= 7;
		*dst-- = tmp;
	}

	return bitl + 2;
}

int32 asn1write_length(byte* dst, uint32 length, byte form) {
	uint32 mask;
	int32 bitl;
	int32 i;

	if (form == UNDEFINITE) {
		*dst++ = 0x80;
		return 1;
	}

	/* construct leading byte */
	if (length < 0x80) {
		*dst = 0x7F & length;
		return 1;

	}

	/* compute the number of subsequent bytes */
	mask = 0xFF << 24;
	bitl = 4;
	while (!(length & mask)) {
		mask >>= 8;
		bitl--;
	}

	*dst++ = 0x80 | bitl;

	/* write subsequent bytes */
	i = bitl - 1;
	while (mask) {
		*dst++ = (length & mask) >> (8 * i--);
		mask >>= 8;
	}

	return bitl + 1;
}

int32 asn1write_endcontent(byte* dst, byte form) {
	if (form != UNDEFINITE)
		return 0;

	*dst++ = 0;
	*dst = 0;

	return 2;
}
