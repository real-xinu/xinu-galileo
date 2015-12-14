#include <xinu.h>

extern uint32 ctr1000;

bool8 c_diffie_hellman(struct sshent *sshb, struct buffer* kex_buffer, DH_GROUPMP* group, hid32 hashid) {
	//
	//#ifdef SSH_DEBUG
	//  int32 mem = memlist.mlength;
	//#endif
	//  byte *buff = (sshb->out).buff;
	//  int32 clength, slength;
	//  MPINT x, e, f;
	//  int32 elength, flength;
	//  int32 nbytes;
	//  bool8 st;
	//  struct dsacert cert;
	//  struct dsasign sign;
	//  byte* shared_secret;
	//  int32 shared_secret_length;
	//  struct hash_state state;
	//
	//  /* init mpint. */
	//  init_mpint(&x);
	//  init_mpint(&e);
	//  init_mpint(&f);
	//
	//  /* Key init protocol */
	//  *buff++ = SSH_MSG_KEXDH_INIT;
	//
	//  /* Genereate a random number. */
	//  if (randmod_mpint(&x, (group->order)->num, &(sshb->randctx)) == FALSE ||
	//
	//      /* Compute e = g^x mod p. */
	//      word_mpint(&e, group->generator, group->size) == FALSE ||
	//      powmod_mpint(&e, &x, group->p, REAL) == FALSE) {
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    return FALSE;
	//  }
	//
	//  /* write e value into the out buffer. */
	//  elength = ssh_write_mpint(buff, &e);
	//
	//  kprintf("AAC\n");
	//  /* send the packet */
	//  if (ssh_tlp_out(sshb, elength + 1) == SYSERR) {
	//#ifdef SSH_DEBUG
	//    kprintf("\ndh_group1_sha1: Fail sending e\n");
	//#endif
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    return FALSE;
	//  }
	//
	//  kprintf("AAD\n");
	//  /* get server answer. */
	//  nbytes = receive();
	//  buff = (sshb->in).buff;
	//  if (nbytes == SYSERR || nbytes == 0 || *buff++ != SSH_MSG_KEXDH_REPLY) {
	//#ifdef SSH_DEBUG
	//    kprintf("\ndh_group1_sha1: Fail receving KEXDH REPLY\n");
	//#endif
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    return FALSE;
	//  }
	//
	//  /* get certificat. */
	//  if ((clength = read_dsscert(buff, &cert)) == SYSERR) {
	//#ifdef SSH_DEBUG
	//    kprintf("\ndh: read certificate error\n");
	//#endif
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    return FALSE;
	//  }
	//
	//#ifdef SSH_DHDEBUG
	//  kprintf("\nCertificate (size %d):\n", clength);
	//  print_mpint((cert.p)->num);
	//  print_mpint(cert.q);
	//  print_mpint(cert.g);
	//  print_mpint(cert.y);
	//#endif
	//
	//#ifdef TODO
	//  kprintf("\ndh_group1_sha1: verify certificate\n");
	//#endif
	//
	//  /* precompute value for the exponanciation. */
	//  if (precompprime_mpint(cert.p) == FALSE ||
	//      /* Copy the certificate into the kex_buffer. */
	//      append_buff(kex_buffer, buff, clength) == FALSE) {
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    clear_dsacert(&cert);
	//    return FALSE;
	//  }
	//  buff += clength;
	//
	//  /* get f value */
	//  if ((flength = ssh_read_mpint(buff, &f)) == SYSERR) {
	//#ifdef SSH_DHDEBUG
	//    kprintf("\ndh: read f error\n");
	//#endif
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    clear_mpint(&f);
	//    clear_dsacert(&cert);
	//    return FALSE;
	//  }
	//
	//  /* copy e and f into the kex_buffer. */
	//  if (append_buff(kex_buffer, (sshb->out).buff+1, elength) == FALSE ||
	//      append_buff(kex_buffer, buff, flength) == FALSE) {
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    clear_mpint(&f);
	//    clear_dsacert(&cert);
	//    return FALSE;
	//  }
	//  buff += flength;
	//
	//  /* get H signature. */
	//  if ((slength = read_dsssign(buff, &sign)) == FALSE) {
	//#ifdef SSH_DEBUG
	//    kprintf("\ndh: read signature error\n");
	//#endif
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    clear_mpint(&f);
	//    clear_dsacert(&cert);
	//    return FALSE;
	//  }
	//  send(sshb->in_proc, TRUE);
	//
	//#ifdef SSH_DHDEBUG
	//  kprintf("\ne value (%d length):\n", elength);
	//  print_mpint(&e);
	//  kprintf("\nf value (%d length):\n", flength);
	//  print_mpint(&f);
	//#endif
	//
	//  /* compute K (shared secret). */
	//  if (powmod_mpint(&f, &x, group->p, REAL) == FALSE ||
	//      /* copy K into the kex_buffer */
	//      kex_buffer->current + (f.order << 2) > kex_buffer->end) {
	//#ifdef SSH_DEBUG
	//    kprintf("\ndh: buffer OutOfMemory\n");
	//#endif
	//    clear_mpint(&x);
	//    clear_mpint(&e);
	//    clear_mpint(&f);
	//    clear_dsacert(&cert);
	//    return FALSE;
	//  }
	//  shared_secret = kex_buffer->current;
	//  shared_secret_length = ssh_write_mpint(kex_buffer->current, &f);
	//  kex_buffer->current += shared_secret_length;
	//
	//#ifdef SSH_DHDEBUG
	//  kprintf("\nkex_buffer:\n");
	//  xdump_array(kex_buffer->buff, (int32)(kex_buffer->current - kex_buffer->buff));
	//
	//  kprintf("\nshared secret K:\n");
	//  print_mpint(&f);
	//#endif
	//
	//
	//#ifdef SSH_DHDEBUG
	//  kprintf("\nH signature value is:\n");
	//  print_mpint(sign.r);
	//  print_mpint(sign.s);
	//#endif
	//
	//  /* compute session id (H) */
	//  hashtab[hashid].init(&state);
	//  hashtab[hashid].update(&state, kex_buffer->buff, (int32)(kex_buffer->current - kex_buffer->buff));
	//  hashtab[hashid].finish(&state, sshb->session_id);
	//  sshb->sid_length = hashtab[hashid].diggest_size;
	//
	//#ifdef SSH_DHDEBUG
	//  kprintf("\nH value is:\n");
	//  xdump_array(sshb->session_id, sshb->sid_length);
	//#endif
	//
	//  /* Verify we computed the good H and generate the keys */
	//  st = TRUE;
	//  if (dsa_check(sshb->session_id, sshb->sid_length, &cert, &sign, SHA1) == FALSE ||
	//      ssh_generate_key(sshb, shared_secret, shared_secret_length, sshb->session_id, hashid) == FALSE) {
	//    st = FALSE;
	//  }
	//
	//  /* Clean up */
	//  clear_mpint(&x);
	//  clear_mpint(&e);
	//  clear_mpint(&f);
	//  clear_dsacert(&cert);
	//  clear_dsasign(&sign);
	//
	//  return st;
	return FALSE;
}

bool8 s_diffie_hellman(struct sshent *sshb, struct buffer* kex_buffer, DH_GROUPMP* group, hid32 hashid) {

	struct ssh_binary_packet* out;
	byte* buff_out;
	struct ssh_binary_packet* in;
	byte* buff_in;
	int32 clength, slength;
	MPINT y, e, f;
	int32 elength, flength;
	bool8 st;
	struct dsasign sign;
	struct hash_state state;
	byte* shared_secret;
	int32 shared_secret_length;

	/* get output buffer */
	if ((out = (struct ssh_binary_packet*) getbuf(Ssh.bpout)) == (struct ssh_binary_packet*) SYSERR) {
		return FALSE;
	}
	buff_out = out->buff;

	/* init mpint. */
	init_mpint(&y);
	init_mpint(&e);
	init_mpint(&f);

	/* Key init protocol */
	*buff_out++ = SSH_MSG_KEXDH_REPLY;

	/* Genereate a random number. */
	wait(sshb->sem_ent);

	if (randmod_mpint(&y, &(group->order), &(sshb->randctx)) == FALSE || \

			/* Compute f = g^y mod p. */
			word_mpint(&f, group->generator, group->size) == FALSE || \
			powmod_mpint(&f, &y, &(group->p), REAL) == FALSE) {
		signal(sshb->sem_ent);
		freebuf((char *) out);
		clear_mpint(&y);
		clear_mpint(&f);
		return FALSE;
	}
	signal(sshb->sem_ent);

	/* write certificat. */
	if ((clength = write_dsscert(buff_out, &xinu_DSAcert)) == SYSERR) {
#ifdef SSH_DEBUG
		kprintf("\ndh: read certificate error\n");
#endif
		freebuf((char *) out);
		clear_mpint(&y);
		clear_mpint(&f);
		return FALSE;
	}

	/* Copy the certificate into the kex_buffer. */
	if (append_buff(kex_buffer, buff_out, clength) == FALSE) {
		freebuf((char *) out);
		clear_mpint(&y);
		clear_mpint(&f);
		return FALSE;
	}
	buff_out += clength;

	/* write f value into the out buffer. */
	flength = ssh_write_mpint(buff_out, &f);

	/* get client init message. */
	if ((in = (struct ssh_binary_packet *)ptrecv(sshb->port)) == (struct ssh_binary_packet *)SYSERR) {
		clear_mpint(&y);
		clear_mpint(&f);
		freebuf((char *) out);
		return FALSE;
	}

	buff_in = in->buff;
	if (*buff_in++ != SSH_MSG_KEXDH_INIT) {
#ifdef SSH_DEBUG
		kprintf("\ndh_group1_sha1: Fail receving KEXDH INIT\n");
#endif
		clear_mpint(&y);
		clear_mpint(&f);
		freebuf((char *)in);
		freebuf((char *) out);
		return FALSE;
	}

	/* get e value */
	if ((elength = ssh_read_mpint(buff_in, &e)) == SYSERR) {
#ifdef SSH_DHDEBUG
		kprintf("\ndh: read e error\n");
#endif
		clear_mpint(&y);
		clear_mpint(&e);
		clear_mpint(&f);
		freebuf((char *)in);
		freebuf((char *) out);
		return FALSE;
	}

	/* copy e and f into the kex_buffer. */
	if (append_buff(kex_buffer, buff_in, elength) == FALSE || \
			append_buff(kex_buffer, buff_out, flength) == FALSE) {
		clear_mpint(&y);
		clear_mpint(&e);
		clear_mpint(&f);
		freebuf((char *)in);
		freebuf((char *) out);
		return FALSE;
	}
	/* don't need incoming packet anymore */
	freebuf((char *)in);

	buff_out += flength;

#ifdef SSH_DHDEBUG
	kprintf("\np value\n");
	print_mpint(&((group->p).num));
	kprintf("\ny value:\n", elength);
	print_mpint(&y);
	kprintf("\ne value (%d length):\n", elength);
	print_mpint(&e);
	kprintf("\nf value (%d length):\n", flength);
	print_mpint(&f);
#endif

	/* compute K (shared secret). */
	if (powmod_mpint(&e, &y, &(group->p), REAL) == FALSE || \
			/* copy K into the kex_buffer */
			kex_buffer->current + (e.order << 2) > kex_buffer->end) {
#ifdef SSH_DEBUG
		kprintf("\ndh: buffer OutOfMemory\n");
#endif
		clear_mpint(&y);
		clear_mpint(&e);
		clear_mpint(&f);
		freebuf((char *) out);
		return FALSE;
	}
	shared_secret = kex_buffer->current;
	shared_secret_length = ssh_write_mpint(kex_buffer->current, &e);
	kex_buffer->current += shared_secret_length;

#ifdef SSH_DHDEBUG
	kprintf("\nK value:\n");
	print_mpint(&e);

	kprintf("\nkex_buffer:\n");
	xdump_array(kex_buffer->buff, (int32)(kex_buffer->current - kex_buffer->buff));

	kprintf("\nshared secret K:\n");
	print_mpint(&e);
#endif

	/* compute session id (H) */
	wait(sshb->sem_ent);

	hashtab[hashid].init(&state);
	hashtab[hashid].update(&state, kex_buffer->buff, (int32)(kex_buffer->current - kex_buffer->buff));
	hashtab[hashid].finish(&state, sshb->session_id);
	sshb->sid_length = hashtab[hashid].diggest_size;

#ifdef SSH_DHDEBUG
	kprintf("\nH value is:\n");
	xdump_array(sshb->session_id, sshb->sid_length);


	kprintf("\nprivate key:\n");
	print_mpint(&((xinu_DSApriv.p).num));
	print_mpint(&(xinu_DSApriv.q));
	print_mpint(&(xinu_DSApriv.g));
	print_mpint(&(xinu_DSApriv.x));
#endif

	/* sign H */
	init_dsasign(&sign);
	if (dsa_sign(sshb->session_id, sshb->sid_length, &xinu_DSApriv, SHA1, &(sshb->randctx), &sign) == FALSE || \
			/* write sign in the out buffer */
			(slength = write_dsssign(buff_out, &sign)) == FALSE) {
#ifdef SSH_DEBUG
		kprintf("\ndh: write signature error\n");
#endif
		signal(sshb->sem_ent);
		clear_mpint(&y);
		clear_mpint(&e);
		clear_mpint(&f);
		clear_dsasign(&sign);
		freebuf((char *) out);
		return FALSE;
	}
	signal(sshb->sem_ent);
	buff_out += slength;

#ifdef SSH_DHDEBUG
	kprintf("\nH signature value is:\n");
	print_mpint(&(sign.r));
	print_mpint(&(sign.s));
#endif

	/* send the packet */
	st = TRUE;
	if (ssh_tlp_out(sshb, out, (int32) (buff_out - out->buff)) == SYSERR || \
			ssh_generate_key(sshb, shared_secret, shared_secret_length, sshb->session_id, hashid) == FALSE) {
#ifdef SSH_DEBUG
		kprintf("\ndh_group1_sha1: Fail sending f or key generation failed\n");
#endif
		st = FALSE;
	}

	/* Clean up */
	clear_mpint(&y);
	clear_mpint(&e);
	clear_mpint(&f);
	clear_dsasign(&sign);
	freebuf((char *) out);

	return st;
}
