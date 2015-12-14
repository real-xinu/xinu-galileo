#include <xinu.h>
/*
 * this file contain the server private keys.
 */

/* DSA */
//HALF_WORD x_dsaval[] = { 0x14e9, 0x1c1c, 0x1150, 0x86cf, 0x9f0c, 0x765b, 0x00d5, 0x6073, 0x8efc, 0x6d6e, 0x0000};
//
//HALF_WORD y_dsaval[] = {0x80b1, 0x09ec, 0x0189, 0xc487, 0x99c4, 0xe635, 0x503d, 0x95f7, 0x4571, 0xd38f, 0x6352, 0xdff8, 0xa025, 0x57ae, 0x1912, 0xf929, 0xf745, 0x0c58, 0x389c, 0x8bb0, 0x570b, 0x4a01, 0x4c95, 0xe2dd, 0x2875, 0x4ef2, 0x600f, 0x94f7, 0xd914, 0x08f6, 0x825e, 0xd198, 0x4d8e, 0x71c8, 0xa6d1, 0x51e0, 0x35ed, 0x8979, 0x8fac, 0x62ef, 0xc63e, 0x2365, 0xe0ca, 0x928e, 0x0665, 0x58f7, 0xb1c7, 0x0151, 0x619b, 0x366e, 0x693e, 0x935b, 0x2725, 0xa766, 0x6cc6, 0x8dde, 0xa79a, 0x1d55, 0xd475, 0x7528, 0x2c34, 0xf49e, 0x952b, 0x52d4, 0x0000};
//
//HALF_WORD p_dsaval[] = {0x4fb9, 0x5648, 0x3097, 0x1acf, 0x91db, 0xe7a8, 0xd393, 0xba83, 0x0b4e, 0x2ce8, 0xb72d, 0x2182, 0xb815, 0xc54d, 0x31dd, 0x1095, 0xdd77, 0xeae4, 0xeb81, 0x58ee, 0xfec6, 0x3e01, 0xfd1b, 0x8b83, 0xa9a2, 0x58af, 0x9ea0, 0x4798, 0xedf6, 0x2e10, 0x7a06, 0x5b5c, 0xa2e5, 0xcf0f, 0x6b59, 0x2fc0, 0x7b22, 0x8e8e, 0xe9a9, 0xd0d0, 0x7d70, 0x4cf1, 0x010b, 0x72c9, 0x712b, 0x41f9, 0xfd34, 0x6da2, 0x8c9d, 0xaba1, 0x15eb, 0x2854, 0xc449, 0xcfee, 0x261c, 0xa051, 0x795f, 0x40c6, 0xc5cb, 0x730b, 0x06bc, 0xf478, 0x87b0, 0x93d1, 0x0000};
//
//HALF_WORD pinv_dsaval[] = {0xf977, 0x8198, 0x7217, 0xa7d8, 0xc0c8, 0x07e8, 0xe963, 0xf134, 0x14ea, 0xda1f, 0x7725, 0xd2ba, 0x2114, 0x7cae, 0xc07c, 0x98c4, 0x9629, 0xd974, 0xc218, 0x49f5, 0x3c9c, 0x0ed3, 0x26c2, 0x24f2, 0x5488, 0xfb1b, 0x0820, 0x3000, 0x799c, 0x63bf, 0x4884, 0xe4c8, 0xea52, 0xf08b, 0x2493, 0xbbf3, 0x959a, 0x1227, 0x3fcc, 0x6019, 0x40c2, 0x1968, 0x7791, 0xc5b0, 0x2e32, 0xeb58, 0x3979, 0xb02b, 0x6bff, 0xb94d, 0x4df8, 0x8cb5, 0xc4da, 0x6d53, 0xefe2, 0x7b9a, 0x4ba9, 0xe59c, 0xd409, 0x5f68, 0x5661, 0xdc78, 0x410d, 0x8d77, 0x0000};
//
//HALF_WORD pr2_dsaval[] = {0x248e, 0x477b, 0x7dbd, 0x1fcf, 0x0fb4, 0xade4, 0xea79, 0xc817, 0x96c8, 0xe06e, 0xa1c0, 0xe541, 0x8aa8, 0xcb8a, 0xccc1, 0x485f, 0x86c2, 0x3574, 0x32fa, 0x0db7, 0x7a01, 0x08d5, 0x27a3, 0xe934, 0x5cc5, 0x74fa, 0x3f85, 0x28c2, 0xc951, 0x163c, 0x61a0, 0x054d, 0x5dea, 0x6656, 0x3424, 0x1591, 0x0c7a, 0x40c7, 0x9a25, 0x6283, 0x6ee8, 0x9d35, 0xede3, 0x29c0, 0x3025, 0x3f8a, 0xe518, 0xd266, 0xb1d8, 0x7db7, 0xd53c, 0xf193, 0xe25e, 0xfda1, 0xdb81, 0x1dc6, 0xd13f, 0x513f, 0x8aa6, 0xbacc, 0xf36e, 0x8f15, 0x10ca, 0x497b, 0x0000};
//
//HALF_WORD q_dsaval[] = {0xe15f, 0xd9f6, 0x3cb6, 0x97f7, 0x7761, 0x20d2, 0xec54, 0x96a8, 0xd61a, 0xef88, 0x0000};
//
//HALF_WORD g_dsaval[] = {0xef54, 0x85eb, 0x877d, 0x7164, 0xdb91, 0xfb22, 0x8852, 0x1854, 0x25d6, 0x6dad, 0x1ac7, 0x4fad, 0x3347, 0xdcde, 0x7a8c, 0x4341, 0x60d5, 0xfb4e, 0xa0aa, 0x96d8, 0x47bf, 0xba77, 0xc341, 0xec6e, 0xcd00, 0x6eaf, 0x79cc, 0x6ae2, 0x85b8, 0xda81, 0x2af1, 0x9719, 0x5181, 0x8d6b, 0x32fe, 0xe8c9, 0xfe30, 0xca16, 0x7312, 0x2b37, 0x0525, 0x49cd, 0x0d61, 0xc265, 0xd545, 0xedf0, 0x2c44, 0x4d83, 0x6fca, 0x0801, 0x6dc9, 0x305c, 0xec5e, 0x10c7, 0x4ff5, 0x5737, 0x07d4, 0xac8f, 0x4c9a, 0x13f4, 0xc2b1, 0xf60b, 0xf666, 0x49bb, 0x0000};
//
struct dsacert xinu_DSAcert = {
	.p = {
		.num = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
		.inv =  {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
		.r_square = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1}
	},
	.q = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
	.y = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
	.g = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1}
};

struct dsapriv xinu_DSApriv = {
	.p = {
		.num = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
		.inv =  {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
		.r_square = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1}
	},
	.q = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
	.x = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1},
	.g = {.val = (HALF_WORD *)SYSERR, .order = 0, .size = 0, .sign = 1}
};

int32 check_anchor(byte* src, char* se, char* type) {
	int32 lse, ltype;

	lse = strnlen(se, 16);
	ltype = strnlen(type, 16);
	if (strncmp((char *)src, DASH, 5) != 0 || \
			strncmp((char *)src + 5, se, lse) != 0 || \
			strncmp((char *)src + 6 + lse, type, ltype) != 0 || \
			strncmp((char *)src + 6 + lse + ltype, DASH, 5) != 0) {
		return SYSERR;
	}

	return 12 + lse + ltype;
}

bool8 extract_dsapriv(byte* buf, int32 len, struct dsapriv* priv, struct dsacert* cert) {
	uint32 id, length;
	byte class, pc;
	int32 tmp;

	// read header
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	// read total length
	tmp = asn1read_length(&length, buf);
	buf += tmp;
	len -= tmp;


	if (length != len) {
#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: invalid total length (%u != %u)\n", length, len);
#endif
		return FALSE;
	}

	// Read first 0
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	tmp = asn1read_length(&length, buf);
	buf += tmp + length;
	len -= tmp + length;

	// read p value
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	tmp = asn1read_length(&length, buf);
	buf += tmp;

	if (*buf == 0) {
		buf++;
		length--;
		len--;
	}
	if (read_bigendian_mpint(&(priv->p.num), buf, length) == FALSE || \
			precompprime_mpint(&(priv->p)) == FALSE || \
			copy_mpprime(&(cert->p), &(priv->p)) == FALSE) {
		clear_dsacert(cert);
		clear_dsapriv(priv);
#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: problem with prime p\n");
#endif
		return FALSE;
	}
	buf += length;
	len -= tmp + length;

	// read q value
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	tmp = asn1read_length(&length, buf);
	buf += tmp;

	if (*buf == 0) {
		buf++;
		length--;
		len--;
	}
	if (read_bigendian_mpint(&(priv->q), buf, length) == FALSE || \
			copy_mpint(&(cert->q), &(priv->q)) == FALSE) {
		clear_dsacert(cert);
		clear_dsapriv(priv);
#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: problem with q\n");
#endif
		return FALSE;
	}
	buf += length;
	len -= tmp + length;

	// read g value
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	tmp = asn1read_length(&length, buf);
	buf += tmp;

	if (*buf == 0) {
		buf++;
		length--;
		len--;
	}
	if (read_bigendian_mpint(&(priv->g), buf, length) == FALSE || \
			copy_mpint(&(cert->g), &(priv->g)) == FALSE) {
		clear_dsacert(cert);
		clear_dsapriv(priv);
#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: problem with g\n");
#endif
		return FALSE;
	}
	buf += length;
	len -= tmp + length;

	// read y value
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	tmp = asn1read_length(&length, buf);
	buf += tmp;

	if (*buf == 0) {
		buf++;
		length--;
		len--;
	}
	if (read_bigendian_mpint(&(cert->y), buf, length) == FALSE) {
		clear_dsacert(cert);
		clear_dsapriv(priv);
#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: problem with y\n");
#endif
		return FALSE;
	}
	buf += length;
	len -= tmp + length;

	// read x value
	tmp = asn1read_identifier(&id, &class, &pc, buf);
	buf += tmp;
	len -= tmp;

	tmp = asn1read_length(&length, buf);
	buf += tmp;

	if (*buf == 0) {
		buf++;
		length--;
		len--;
	}
	if (read_bigendian_mpint(&(priv->x), buf, length) == FALSE) {
		clear_dsacert(cert);
		clear_dsapriv(priv);
#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: problem with x\n");
#endif
		return FALSE;
	}
	buf += length;
	len -= tmp + length;

	if (len == 0) {
		return TRUE;
	}

#ifdef SSH_DEBUG
		kprintf("extract_dsapriv: data remain\n");
#endif
	clear_dsacert(cert);
	clear_dsapriv(priv);

	return FALSE;
}

bool8 ssh_load_dsakey(struct dsapriv* priv, struct dsacert* cert, char* dir, char* filename) {

	int32 file, fsize;
	int32 next, lsize;
	byte* fptr;
	byte* keyblob, *wt;
	bool8 st;

	char path[128];

	memset(path, 0, sizeof(path));
	next = strcopy((byte *)path, dir);
	strcopy((byte *)path + next, filename);

	if ((file = open(SSH_FILES, path, "ro")) == SYSERR) {
#ifdef SSH_DEBUG
		kprintf("ssh_load_dsakey: Fail opening file\n");
#endif
		return FALSE;
	}

	// Read complete file
	int32 tmp = -1;
	if ((fsize = control(SSH_FILES, RFS_CTL_SIZE, file, 0)) == SYSERR ||
			(fptr = (byte *)getmem(fsize)) == (byte *) SYSERR ||
			(tmp = read(file, (char *)fptr, fsize)) != fsize) {
		kprintf("ssh_load_dsakey: cannot read file.\n");
		close(file);
		return FALSE;
	}
	close(file);

	// Decode base64
	// read header
	if ((next = check_anchor(fptr, BEGIN, DSA_PRIVKEY_HEADER)) == SYSERR || \
			(keyblob = (byte *)getmem(BASE64_DECODED_SIZE(fsize))) == (byte *)SYSERR) {
		kprintf("ssh_load_dsakey: Invalid file format (BEGINING).\n");
		return FALSE;
	}

	// decode body
	wt = fptr + next;
	next = 0;
	while (wt < fptr + fsize && check_anchor(wt, END, DSA_PRIVKEY_HEADER) == SYSERR) {
		for (lsize = 1 ; lsize < LINE_LENGTH && wt[lsize-1] != '\n' ; lsize++);
		next += base64_decode(wt, lsize, keyblob + next);
		wt += lsize;
	}
	freemem((char *)fptr, fsize);

	if (wt >= fptr + fsize) {
#ifdef SSH_DEBUG
		kprintf("ssh_load_dsakey: Invalid file format (END).\n");
#endif
		freemem((char *)keyblob, BASE64_DECODED_SIZE(fsize));
		return FALSE;
	}

	// parse ASN.1 key blob
	st = extract_dsapriv(keyblob, next, priv, cert);

	// clean up
	freemem((char *)keyblob, BASE64_DECODED_SIZE(fsize));

	return st;
}
