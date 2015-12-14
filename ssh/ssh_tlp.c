#include <xinu.h>

bool8 ssh_tlp_out(struct sshent *sshb, struct ssh_binary_packet* out, uint32 payload_length) {

	struct ciphent *cipher;
	int32 length_padded;
	byte padding, i;
	uint32 *padd;
	int32 block_size;
	struct hash_state state;


	/* Get the entry mutual exclusion */
	wait(sshb->sem_ent);

	/* Initialisation */
	cipher = &(ciphtab[CIPHER(sshb->ciphid_out)]);
	block_size = cipher->block_size > 8 ? cipher->block_size : 8;
	padd = (uint32 *) (out->buff + payload_length);

	/* Compute padding length. */
	length_padded = (uint32)roundm(payload_length + HEADER_SIZE + MIN_PADDING, block_size);

	/* Update header */
	out->packet_length = (uint32) htonl(length_padded - 4);
	padding = 0xff & (length_padded - payload_length - HEADER_SIZE);
	out->padding_length = padding;

	/* Add random padding. (Maybe more than necessary) */
	for (i = 0 ; i < padding ; i += sizeof(uint32)) {
		*padd++ = crand(&(sshb->randctx));
	}

#ifdef SSH_DEBUG
	kprintf("\nssh_tlc_out: Packet before MAC/ENC: (seq number %d)\n", sshb->seq_out);
	xdump_binary_packet(out, FALSE, 0);
#endif

	/* Mac */
	if (STATUS(sshb->state) > IN_CLEAR) {

		hmac_init(sshb->macid_out, &state, sshb->mackey_out, hashtab[sshb->macid_out].key_size);
		/* WARNING hack!!!!!! -> should make a buffer and call hmac_update... */
		state.block[0] = htonl(sshb->seq_out);
		state.current += 4;
		state.current_size += 4;
		state.block_used = 4;
		hmac_update(sshb->macid_out, &state, (byte *) out, length_padded);
		hmac_finish(sshb->macid_out, &state, sshb->mackey_out, hashtab[sshb->macid_out].key_size, (byte *)out + length_padded);

		/* Encrypt. */
		modetab[MODE(sshb->ciphid_out)].enc(CIPHER(sshb->ciphid_out), sshb->iv_out, sshb->enckeyblob_out, (byte *) out, (byte *) out, length_padded);

		length_padded += hashtab[sshb->macid_out].diggest_size;
	}

#ifdef SSH_DEBUG
	kprintf("\nssh_tlc_out: Packet after MAC/ENC:\n");
	xdump_binary_packet(out, TRUE, length_padded);
#endif
	/* Increment sequence number */
	sshb->seq_out++;

	length_padded -= tcp_send(sshb->tcp_id, (char *)out, length_padded);

	signal(sshb->sem_ent);

	return !length_padded;
}

bool8 ssh_tlp_in(struct sshent *sshb, struct ssh_binary_packet* in) {

	struct hash_state   state;
	byte                diggest[MAX_HASH_DIGGEST];
	int32               nbytes;
	int32               block_size, diggest_size;

	block_size = 0;
	/* max byte to receive */
	nbytes = BUFFERS_LENGTH + HEADER_SIZE;

	/* get first block to compute message packet */
	wait(sshb->sem_ent);
	if (STATUS(sshb->state) > IN_CLEAR) {
		block_size = ciphtab[CIPHER(sshb->ciphid_in)].block_size;
		diggest_size = hashtab[sshb->macid_in].diggest_size;
		signal(sshb->sem_ent);

		if (tcp_recv(sshb->tcp_id, (char *) &(sshb->in), block_size) != block_size) {
#ifdef SSH_DEBUG
			kprintf("ssh_tlp_in: Fail receiving (1)\n");
#endif
			return FALSE;
		}

		/* decrypt */
		wait(sshb->sem_ent);
		modetab[MODE(sshb->ciphid_in)].dec(CIPHER(sshb->ciphid_in), sshb->iv_in, sshb->enckeyblob_in, (byte *) &(sshb->in), (byte *)in, block_size);
		signal(sshb->sem_ent);

		/* compute number of byte remaining */
		nbytes = (int32) ntohl(in->packet_length) + 4 + diggest_size - block_size;
	} else {
		signal(sshb->sem_ent);
	}

	/* receiving remaining data */
	if ((nbytes = tcp_recv(sshb->tcp_id, (char *) &(sshb->in) + block_size, nbytes)) == SYSERR || nbytes <= 0) {
#ifdef SSH_DEBUG
		kprintf("ssh_tlp_in: Fail receiving (2)\n");
#endif
		return FALSE;
	}

#ifdef SSH_DEBUG
	kprintf("\nssh_tlp_in: Packet before MAC/DEC (%d bytes received) (#%d):\n", nbytes + block_size, sshb->seq_in);
	xdump_binary_packet(&(sshb->in), TRUE, nbytes + block_size);
#endif

	wait(sshb->sem_ent);
	if (STATUS(sshb->state) > IN_CLEAR) {

		/* the MAC is not encrypted */
		nbytes -= diggest_size;

		if (nbytes + block_size > SSH_BUFF_SIZE) {
			panic("buffer size too small\n");
		}

		/* Decrypt. */
		modetab[MODE(sshb->ciphid_in)].dec(CIPHER(sshb->ciphid_in), sshb->iv_in, sshb->enckeyblob_in, (byte *) &(sshb->in) + block_size, (byte *)in + block_size, nbytes);

		/* add first block already received */
		nbytes += block_size;

		/* verify MAC */
		hmac_init(sshb->macid_in, &state, sshb->mackey_in, hashtab[sshb->macid_in].key_size);
		/* WARNING hack!!!!!! -> should make a buffer and call hmac_update... */
		state.block[0] = htonl(sshb->seq_in);
		state.current += 4;
		state.current_size += 4;
		state.block_used = 4;
		hmac_update(sshb->macid_in, &state, (byte *) in, nbytes);
		hmac_finish(sshb->macid_in, &state, sshb->mackey_in, hashtab[sshb->macid_in].key_size, diggest);

		if (memcmp(diggest, (byte *) &(sshb->in) + nbytes, diggest_size) != 0) {
			signal(sshb->sem_ent);
			ssh_disconnect(sshb, SSH_DISCONNECT_MAC_ERROR, "MAC Error", 9);
#ifdef SSH_DEBUG
			kprintf("\nssh_tlp_in: MAC incorrect.\n");
			kprintf("\nmessage decrypted:\n");
			xdump_binary_packet((struct ssh_binary_packet *)in, TRUE, nbytes);

			kprintf("\nMAC computed:\n");
			xdump_array(diggest, hashtab[sshb->macid_in].diggest_size);
#endif
			return FALSE;
		}
	} else {
		if (nbytes > SSH_BUFF_SIZE) {
			panic("buffer size too small\n");
		}
		memcpy(in, (byte *) &(sshb->in), nbytes);
	}

	/* Increment sequence number */
	sshb->seq_in++;

#ifdef SSH_DEBUG
	kprintf("\nssh_tlc_in: Packet after MAC/DEC (%d bytes received, %u):\n", nbytes, in->packet_length);
	xdump_binary_packet((struct ssh_binary_packet *)in, FALSE, 0);
#endif

	/* release mutex */
	signal(sshb->sem_ent);

	/* Update packet length to be length of payload */
	in->packet_length = ntohl(in->packet_length) - in->padding_length - 1;

	return TRUE;
}
