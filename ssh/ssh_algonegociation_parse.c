#include <xinu.h>

/*-------------------------------------------------------------------------------------
 * parse_kex_algo - from a string returns the corresponding unique identifier
 *   supported algorithm so far with expected output (2/23/2015)
 *    - diffie-hellman-group14-sha1     DHG14 << 16 | SHA1
 *    - diffie-hellman-group14-sha1-96  DHG14 << 16 | SHA1_96
 *    - diffie-hellman-group1-sha1      DHG1 << 16 | SHA1
 *    - diffie-hellman-group1-sha1-96   DHG1 << 16 | SHA1_96
 *
 *    return SYSERR otherwise
 *-------------------------------------------------------------------------------------
 */
int32 parse_kex_algo(char* name, int32 len) {
	int32 group, hash, diggest;

	/* get method */
	if (len < 20 || strncmp(name, "diffie-hellman-group", 20) != 0) {
		return SYSERR;
	}

	name += 20;
	len -= 20;

	group = 0;
	while (len > 0 && *name >= '0' && *name <= '9') {
		group = group * 10 + (*name++ - '0');
		len--;
	}

	switch (group) {
		case 1:
			group = DHG1;
			break;
		case 14:
			group = DHG14;
			break;
		default:
			return SYSERR;
	}

	if (len < 4 || strncmp(name, "-sha", 4) != 0) {
		return SYSERR;
	}

	name += 4;
	len -= 4;

	hash = diggest = 0;
	while (len-- > 0 && *name >= '0' && *name <= '9') {
		hash = hash * 10 + (*name++ - '0');
	}

	if (*name == '-') {
		name++;
		while (len-- > 0 && *name >= '0' && *name <= '9') {
			diggest = diggest * 10 + (*name++ - '0');
		}
	}

	if (len > 0)
		return SYSERR;

	switch (hash * 1000 + diggest) {
		case 1000:
			return KEX_ID(group, SHA1);
		case 1096:
			return KEX_ID(group, SHA1_96);
		default:
			return SYSERR;
	}

}

/*-------------------------------------------------------------------------------------
 * parse_pke_algo - from a string returns the corresponding unique identifier
 *   supported algorithm so far with expected output (2/23/2015)
 *    - ssh-dss      DSS
 *
 *    return SYSERR otherwise
 *-------------------------------------------------------------------------------------
 */
int32 parse_pke_algo(char* name, int32 len) {

	/* get method */
	if (len < 4 || strncmp(name, "ssh-", 4) != 0)
		return SYSERR;

	name += 4;
	len -= 4;

	if (len < 3)
		return SYSERR;

	if (*name == 'd' && *(name + 1) == 's' && *(name + 2) == 's')
		return DSS;

	//  if (*name == 'r' && *(name + 1) == 's' && *(name + 2) == 'a')
	//    return RSA;

	return SYSERR;
}

/*-------------------------------------------------------------------------------------
 * parse_enc_algo - from a string returns the corresponding unique identifier
 *   supported algorithm so far with expected output (3/19/2015)
 *    - 3des-cbc  CIPH_ID(TDEA, CBC)
 *    - none      NONE
 *
 *    return SYSERR otherwise
 *-------------------------------------------------------------------------------------
 */
int32 parse_enc_algo(char * name, int32 len) {

	/* get cipher */
	//  if (len == 10 && strncmp(name, "aes128-cbc", 10) == 0) {
	//    return CIPH_ID(AES128, CBC);
	//  }

	if (len == 8 && strncmp(name, "3des-cbc", 8) == 0) {
		return CIPH_ID(TDEA, CBC);
	}

	if (len == 4 && strncmp(name, "none", 4) == 0) {
		return NONE;
	}

	return SYSERR;
}

/*-------------------------------------------------------------------------------------
 * parse_mac_algo - from a string returns the corresponding unique identifier
 *   supported algorithm so far with expected output (2/23/2015)
 *    - hmac-sha1    SHA1
 *    - hmac-sha1-96 SHA1_96
 *
 *    return SYSERR otherwise
 *-------------------------------------------------------------------------------------
 */
int32 parse_mac_algo(char * name, int32 len) {
	int32 hash, diggest;

	/* it is an hmac-* */
	if (len > 8 &&  strncmp(name, "hmac-sha", 8) == 0) {
		name += 8;
		len -= 8;

		hash = diggest = 0;
		while (len-- > 0 && *name >= '0' && *name <= '9') {
			hash = hash * 10 + (*name++ - '0');
		}

		if (*name == '-') {
			name++;
			while (len-- > 0 && *name >= '0' && *name <= '9') {
				diggest = diggest * 10 + (*name++ - '0');
			}
		}

		if (len > 0)
			return SYSERR;

		switch (hash * 1000 + diggest) {
			case 1000:
				return SHA1;
			case 1096:
				return SHA1_96;
			default:
				return SYSERR;
		}
	}

	/* maybe none ? */
	if (len == 4 && strncmp(name, "none", 4) == 0) {
		return NONE;
	}

	return SYSERR;
}

/*-------------------------------------------------------------------------------------
 * parse_com_algo - from a string returns the corresponding unique identifier
 *   supported algorithm so far with expected output (2/23/2015)
 *    - none  NONE
 *
 *    return SYSERR otherwise
 *-------------------------------------------------------------------------------------
 */
int32 parse_com_algo(char * name, int32 len) {

	/* get cipher */
	/* only none is supported currently */
	if (len < 4 || strncmp(name, "none", 4) != 0) {
		return SYSERR;
	}

	return NONE;
}
