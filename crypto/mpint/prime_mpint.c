#include <xinu.h>

MPPRIME* new_mpprime() {
	MPPRIME* ret = (MPPRIME *)getmem(sizeof(MPPRIME));

	init_mpprime(ret);

	return ret;
}

/* The pow function require precomputed values in the MPPRIME struc.
 * This function initialized them.
 */
bool8 precompprime_mpint(MPPRIME* prime) {

	MPINT R, tmp;

	init_mpint(&R);
	init_mpint(&tmp);
	/* compute -P^(-1) mod R */
	/* R is the smallest power of 2 higher than P */
	/* Copy p */
	if (copy_mpint(&tmp, &(prime->num)) == FALSE || \

			/* Create R */
			word_mpint(&R, 0, (tmp.size - 1) << 1) == FALSE) {
		clear_mpint(&R);
		clear_mpint(&tmp);
		return FALSE;
	}
	/* set value of R */
	(&R)->val[(prime->num).size - 1] = 1;
	(&R)->order = (prime->num).size;

	/* compute */
	if (invmod_mpint(&tmp, &R) == FALSE ||
			diff_mpint(&tmp, &R, -1) == FALSE ||
			subval_mpint(&(prime->inv), 0, &tmp, 0, prime->num.size - 1) == FALSE) {
		clear_mpint(&R);
		clear_mpint(&tmp);
		return FALSE;
	}
	clear_mpint(&tmp);

	/* Compute R^2 mod P */
	/* Create R^2 */
	(&R)->val[(prime->num).size - 1] = 0;
	(&R)->val[((prime->num).size - 1) << 1] = 1;
	(&R)->order = (((prime->num).size - 1) << 1) + 1;

	/* compute R^2 mod P */
	if (unsignmod_mpint(&(prime->r_square), &R, &(prime->num)) == FALSE) {
		clear_mpint(&R);
		clear_mpint(&(prime->inv));
		clear_mpint(&(prime->r_square));
		return FALSE;
	}

	/* clean up */
	clear_mpint(&R);
	return TRUE;
}

void delete_mpprime(MPPRIME *val) {
	if (val != (MPPRIME *)SYSERR) {
		clear_mpint(&(val->num));
		clear_mpint(&(val->inv));
		clear_mpint(&(val->r_square));

		freemem((char *)val, sizeof(MPPRIME));
	}
}

