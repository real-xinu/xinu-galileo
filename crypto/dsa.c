#include <xinu.h>

void init_dsacert(struct dsacert* certificate) {
	init_mpprime(&(certificate->p));
	init_mpint(&(certificate->q));
	init_mpint(&(certificate->g));
	init_mpint(&(certificate->y));
}

void init_dsasign(struct dsasign* sign) {
	init_mpint(&(sign->r));
	init_mpint(&(sign->s));
}

void init_dsapriv(struct dsapriv* priv) {
	init_mpprime(&(priv->p));
	init_mpint(&(priv->q));
	init_mpint(&(priv->g));
	init_mpint(&(priv->x));
}

bool8 dsa_check(byte* msg, int32 length, struct dsacert *cert, hid32 hashid, struct dsasign *sign) {

	bool8 st = TRUE;
	MPINT tmp, hash_mpint, u1, u2, w, cg, cy;
	byte hash[MAX_HASH_DIGGEST];
	struct hash_state state;

	/* Compute hash of the message */
	hashtab[hashid].init(&state);
	hashtab[hashid].update(&state, msg, length);
	hashtab[hashid].finish(&state, hash);

	/* hash as mpint. */
	init_mpint(&hash_mpint);
	if (read_bigendian_mpint(&hash_mpint, hash, hashtab[hashid].diggest_size) == FALSE) {
		clear_mpint(&hash_mpint);
		return FALSE;
	}

	init_mpint(&w);
	init_mpint(&tmp);
	init_mpint(&u1);
	init_mpint(&u2);
	init_mpint(&cg);
	init_mpint(&cy);

	if (unsigncmp_mpint(&(sign->s), &(cert->q)) >= 0 || \
			unsigncmp_mpint(&(sign->r), &(cert->q)) >= 0 || \

			/* inverse s */
			copy_mpint(&w, &(sign->s)) == FALSE || \
			invmod_mpint(&w, &(cert->q)) == FALSE || \

			/* compute u1. */
			mul_mpint(&tmp, &hash_mpint, &w, FALSE) == FALSE || \
			unsignmod_mpint(&u1, &tmp, &(cert->q)) == FALSE || \

			/* compute u2 */
			word_mpint(&tmp, 0, 1) == FALSE || \
			mul_mpint(&tmp, &(sign->r), &w, FALSE) == FALSE || \
			unsignmod_mpint(&u2, &tmp, &(cert->q)) == FALSE || \
			/* Compute MONTGOMERY(g^u1 mod p) */
			copy_mpint(&cg, &(cert->g)) == FALSE || \
			powmod_mpint(&cg, &u1, &(cert->p), MONTGOMERY) == FALSE || \

			/* Compute MONTGOMERY(y^u2 mod p) */
			copy_mpint(&cy, &(cert->y)) == FALSE || \
			powmod_mpint(&cy, &u2, &(cert->p), MONTGOMERY) == FALSE || \
			/* Compute MONTGOMERY(g^u1 * y^u2 mod p) */
			montgomery_mpint(&cg, &cy, &((cert->p).num), &((cert->p).inv)) == FALSE || \

			/* Go back to real world */
			montgomery_inv_mpint(&cg, &((cert->p).num), &((cert->p).inv)) == FALSE || \

			/* (g^u1 * y^u2 mod p) mod q */
			unsignmod_mpint(&tmp, &cg, &(cert->q)) == FALSE || \

			/* Perform check */
			unsigncmp_mpint(&tmp, &(sign->r)) != 0) {
				st = FALSE;
			}

	// Clean up
	clear_mpint(&tmp);
	clear_mpint(&hash_mpint);
	clear_mpint(&u1);
	clear_mpint(&u2);
	clear_mpint(&w);
	clear_mpint(&cg);
	clear_mpint(&cy);

	return st;
}

bool8 dsa_sign(byte* msg, int32 length, struct dsapriv *priv, hid32 hashid, randctx* ctx, struct dsasign *sign) {

	MPINT k, gc, hash_mpint;
	byte hash[MAX_HASH_DIGGEST];
	struct hash_state state;
	bool8 st = TRUE;

	/* Compute hash of the message */
	hashtab[hashid].init(&state);
	hashtab[hashid].update(&state, msg, length);
	hashtab[hashid].finish(&state, hash);

	/* hash as mpint. */
	init_mpint(&hash_mpint);
	if (read_bigendian_mpint(&hash_mpint, hash, hashtab[hashid].diggest_size) == FALSE) {
		clear_mpint(&hash_mpint);
		return FALSE;
	}

	init_mpint(&gc);
	init_mpint(&k);

	/* generate k */
	if (randmod_mpint(&k, &(priv->q), ctx) == FALSE || \

			/* compute r */
			copy_mpint(&gc, &(priv->g)) == FALSE || \
			powmod_mpint(&gc, &k, &(priv->p), REAL) == FALSE || \
			unsignmod_mpint(&(sign->r), &gc, &(priv->q)) == FALSE || \

			/* compute s */
			invmod_mpint(&k, &(priv->q)) == FALSE || \
			mul_mpint(&hash_mpint, &(priv->x), &(sign->r), FALSE) == FALSE || \
			word_mpint(&gc, 0, 1) == FALSE || \
			mul_mpint(&gc, &k, &hash_mpint, FALSE) == FALSE || \
			unsignmod_mpint(&(sign->s), &gc, &(priv->q)) == FALSE) {
		st = FALSE;
	}

	clear_mpint(&k);
	clear_mpint(&gc);
	clear_mpint(&hash_mpint);

	return st;
}

void clear_dsacert(struct dsacert* cert) {
	clear_mpprime(&(cert->p));
	clear_mpint(&(cert->q));
	clear_mpint(&(cert->g));
	clear_mpint(&(cert->y));
}

void clear_dsasign(struct dsasign* sign) {
	clear_mpint(&(sign->s));
	clear_mpint(&(sign->r));
}

void clear_dsapriv(struct dsapriv* priv) {
	clear_mpprime(&(priv->p));
	clear_mpint(&(priv->q));
	clear_mpint(&(priv->g));
	clear_mpint(&(priv->x));
}

