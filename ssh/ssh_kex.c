#include <xinu.h>

bool8 key_expansion(struct sshent* sshb, struct buffer* hbuff, hid32 hashid, byte* letter, byte* dst, int32 length);

extern uint32 ctr1000;

/*-------------------------------------------------------------------------------------
 * ssh_key_exchange - perform the key exchange algorithm negociated previously.
 *-------------------------------------------------------------------------------------
 */
bool8 ssh_key_exchange(struct sshent* sshb, struct buffer *kex_buff) {
	DH_GROUPMP* group;

	switch (GROUP(sshb->kexid)) {
		case DHG1:
			group = &DH_G1;
			break;
		case DHG14:
			group = &DH_G14;
			break;
		default:
			return FALSE;
	}

	wait(sshb->sem_ent);
	switch (SIDE(sshb->state)) {
		case SSH_CLIENT:
			signal(sshb->sem_ent);
			return c_diffie_hellman(sshb, kex_buff, group, HASH(sshb->kexid));
		case SSH_SERVER:
			signal(sshb->sem_ent);
			return s_diffie_hellman(sshb, kex_buff, group, HASH(sshb->kexid));
		default: // should never happen
			signal(sshb->sem_ent);
			return FALSE;
	}
}

bool8 ssh_generate_key(struct sshent* sshb, byte* shared_secret, int32 length, byte* H, hid32 hashid) {

	struct buffer gk_buff;
	byte* letter;
	byte keytmp[MAX_KEYS_SIZE];

	/* Construct Buffer and fill it */
	if (allocate_buff(&gk_buff, length + 4 * hashtab[hashid].diggest_size + 1) == FALSE)
		return FALSE;

	/* K || H */
	if (append_buff(&gk_buff, shared_secret, length) == FALSE || \
			append_buff(&gk_buff, H, hashtab[hashid].diggest_size) == FALSE) {
		clear_buffer(&gk_buff);
		return FALSE;
	}

	/* Add letter */
	if (gk_buff.current >= gk_buff.end) {
		clear_buffer(&gk_buff);
		return FALSE;
	}
	letter = gk_buff.current++;

	/* add session_id */
	if (wait(sshb->sem_ent) == FALSE || \
			append_buff(&gk_buff, sshb->session_id, sshb->sid_length) == FALSE) {
		signal(sshb->sem_ent);
		clear_buffer(&gk_buff);
		return FALSE;
	}

	/* generate iv in */
	*letter = SIDE(sshb->state) == SSH_SERVER ? 'A' : 'B';
	if (key_expansion(sshb, &gk_buff, hashid, letter, sshb->iv_in, ciphtab[CIPHER(sshb->ciphid_in)].block_size) == FALSE) {
		clear_buffer(&gk_buff);
		signal(sshb->sem_ent);
		return FALSE;
	}

	/* generate iv out */
	*letter = SIDE(sshb->state) == SSH_SERVER ? 'B' : 'A';
	if (key_expansion(sshb, &gk_buff, hashid, letter, sshb->iv_out, ciphtab[CIPHER(sshb->ciphid_out)].block_size) == FALSE) {
		clear_buffer(&gk_buff);
		signal(sshb->sem_ent);
		return FALSE;
	}

	/* generate encryption key in */
	*letter = SIDE(sshb->state) == SSH_SERVER ? 'C' : 'D';
	if (key_expansion(sshb, &gk_buff, hashid, letter, keytmp, ciphtab[CIPHER(sshb->ciphid_in)].key_size) == FALSE || \
			(sshb->enckeyblob_in = (byte *)getmem(ciphtab[CIPHER(sshb->ciphid_in)].keyblob_size)) == (byte *)SYSERR) {
		clear_buffer(&gk_buff);
		signal(sshb->sem_ent);
		return FALSE;
	}
	ciphtab[CIPHER(sshb->ciphid_in)].gen(keytmp, sshb->enckeyblob_in);

	/* generate encryption key out */
	*letter = SIDE(sshb->state) == SSH_SERVER ? 'D' : 'C';
	if (key_expansion(sshb, &gk_buff, hashid, letter, keytmp, ciphtab[CIPHER(sshb->ciphid_out)].key_size) == FALSE || \
			(sshb->enckeyblob_out = (byte *)getmem(ciphtab[CIPHER(sshb->ciphid_in)].keyblob_size)) == (byte *)SYSERR) {
		clear_buffer(&gk_buff);
		signal(sshb->sem_ent);
		return FALSE;
	}
	ciphtab[CIPHER(sshb->ciphid_out)].gen(keytmp, sshb->enckeyblob_out);
	memset(keytmp, 0, MAX_KEYS_SIZE);

	/* generate integrity key in */
	*letter = SIDE(sshb->state) == SSH_SERVER ? 'E' : 'F';
	if (key_expansion(sshb, &gk_buff, hashid, letter, sshb->mackey_in, hashtab[sshb->macid_in].key_size) == FALSE) {
		clear_buffer(&gk_buff);
		signal(sshb->sem_ent);
		return FALSE;
	}

	/* generate integrity key out */
	*letter = SIDE(sshb->state) == SSH_SERVER ? 'F' : 'E';
	if (key_expansion(sshb, &gk_buff, hashid, letter, sshb->mackey_out, hashtab[sshb->macid_out].key_size) == FALSE) {
		clear_buffer(&gk_buff);
		signal(sshb->sem_ent);
		return FALSE;
	}

	signal(sshb->sem_ent);
	clear_buffer(&gk_buff);

	return TRUE;
}

bool8 key_expansion(struct sshent* sshb, struct buffer* hbuff, hid32 hashid, byte* letter, byte* dst, int32 length) {
	byte diggest[MAX_HASH_DIGGEST];
	byte *wtdst, *wtsrc, *next;
	int32 i, j;
	struct hash_state state;
#ifdef SSH_DHDEBUG
	byte ll = *letter;
#endif

	if (length > MAX_KEYS_SIZE) {
#ifdef SSH_DHDEBUG
		kprintf("key_expansion: key length to big.\n");
#endif
		return FALSE;
	}

	i = 0;
	wtdst = dst;
	next = letter;
	while (TRUE) {
		/* hash the buffer */
		hashtab[hashid].init(&state);
		hashtab[hashid].update(&state, hbuff->buff, (int32)(hbuff->current - hbuff->buff));
		hashtab[hashid].finish(&state, diggest);
		wtsrc = diggest;
		for (j = 0 ; j < hashtab[hashid].diggest_size && i < length ; i++, j++) {
			*wtdst++ = *wtsrc++;
		}
		if (i >= length)
			break;

		/* Update the buffer */
		hbuff->current = next;
		if (append_buff(hbuff, diggest, hashtab[hashid].diggest_size) == FALSE) {
			return FALSE;
		}
		next = hbuff->current;
	}

	/* if the key had to be extanded, we reset the buffer */
	if (next != letter) {
		hbuff->current = letter + 1;
		if (append_buff(hbuff, sshb->session_id, sshb->sid_length) == FALSE) {
			return FALSE;
		}
	}

#ifdef SSH_DHDEBUG
	kprintf("Key generated for letter '%c':\n", ll);
	xdump_array(dst, length);
#endif

	return TRUE;
}
