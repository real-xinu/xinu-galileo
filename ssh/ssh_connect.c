#include <xinu.h>

sshid32 nextsshid();
process ssh_tlp_establish(struct sshent* sshb);

/*-------------------------------------------------------------------------------------
 * ssh_accept - acceptss an incoming connection
 *-------------------------------------------------------------------------------------
 */
sshid32 ssh_accept(int32 tcp_id) {

	sshid32 sshid;
	struct sshent *sshb;

	wait(Ssh.sshmutex);

	/* Get next available block. */
	sshid = nextsshid();
	if (sshid == SYSERR) {
		tcp_close(tcp_id);
		signal(Ssh.sshmutex);
#ifdef SSH_DEBUG
		kprintf("ssh_connect: no more ssh blocks available.\n");
#endif
		return SYSERR;
	}

	sshb = &(sshtab[sshid]);

	/* create entry mutex */
	if ((sshb->sem_ent = semcreate(1)) == SYSERR) {
		tcp_close(tcp_id);
		signal(Ssh.sshmutex);
#ifdef SSH_DEBUG
		kprintf("ssh_connect: fail creating mutex.\n");
#endif
		return SYSERR;
	}

	/* open a port for inter process communication */
	if ((sshb->port = ptcreate(SSH_PORT_SIZE)) == SYSERR) {
		tcp_close(tcp_id);
		semdelete(sshb->sem_ent);
		signal(Ssh.sshmutex);
#ifdef SSH_DEBUG
		kprintf("ssh_connect: fail opening a port.\n");
#endif
		return SYSERR;
	}

	sshb->tcp_id = tcp_id;

	/* Mark entry used. */
	sshb->state = SSH_SERVER;

	/* seed the PRGN context */
	ssh_initnewctx(&(sshb->randctx), &(Ssh.sshctx));

	/* Establish SSH Transport Layer Protocol and start service. */
	sshb->user_proc = create(ssh_tlp_establish, SSH_SERVER_STACK_SIZE, SSH_SERVER_USER_PRIO, "ssh_establish", 1, sshb, NULL);
	sshb->in_proc = getpid();

	resume(sshb->user_proc);

	/* release mutex. */
	signal(Ssh.sshmutex);

#ifdef SSH_DEBUG
		kprintf("ssh_connect: success.\n");
#endif

	return sshid;
}


/*-------------------------------------------------------------------------------------
 * ssh_connect - create a SSH connexion as a SSH client
 *-------------------------------------------------------------------------------------
 */
sshid32 ssh_connect(uint32 addr, uint16 port) {

	//  sshid32 sshid;
	//  struct sshent *sshb;
	//
	//  wait(Ssh.sshmutex);
	//
	//  /* Get next available block. */
	//  sshid = nextsshid();
	//  if (sshid == SYSERR) {
	//#ifdef SSH_DEBUG
	//    kprintf("ssh_connect: no more ssh blocks available.\n");
	//#endif
	//    return SYSERR;
	//  }
	//
	//  sshb = &(sshtab[sshid]);
	//
	//  /* Open connection. */
	//#ifdef SSH_DEBUG
	//  kprintf("ssh_connect: connect to %d.%d.%d.%d\n", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);
	//#endif
	//  sshb->tcp_id = tcp_register(addr, port, 1);
	//
	//  if (sshb->tcp_id == SYSERR) {
	//    ssh_clean_entry(sshb);
	//#ifdef SSH_DEBUG
	//    kprintf("ssh_connect: fail open TCP connection\n");
	//#endif
	//    return SYSERR;
	//  }
	//
	//  /* Mark entry used. */
	//  sshb->state = SSH_CLIENT;
	//
	//  /* seed the PRGN context */
	//  ssh_initnewctx(&(sshb->randctx), &(Ssh.sshctx));
	//
	//  /* release mutex. */
	//  signal(Ssh.sshmutex);
	//
	//  /* Establish SSH connection. */
	//  if (ssh_tlp_establish(sshb) == TRUE) {
	//#ifdef SSH_DEBUG
	//    kprintf("ssh_connect: Success\n");
	//#endif
	//    return sshid;
	//  }
	//#ifdef SSH_DEBUG
	//  kprintf("ssh_connect: Fail\n");
	//#endif
	return SYSERR;
}

/*-------------------------------------------------------------------------------------
 * ssh_tlp_establish - establishs the Transport Layer Protocol (RFC 4253)
 *-------------------------------------------------------------------------------------
 */
process ssh_tlp_establish(struct sshent* sshb) {

	struct buffer kex_buff;

	/* Allocate buffer for key exchange */
	if (allocate_buff(&kex_buff, MAX_KEX_BUFF_LENGTH) == FALSE || \

			/* perform version exchange */
			ssh_version_exchange(sshb, &kex_buff) == FALSE || \

			/* start using binary packet protocol (Now mutual exclusion is needed) */
			send(sshb->in_proc, TRUE) == SYSERR || \

			/* Perform algorithm negociation */
			ssh_algorithms_negociation(sshb, &kex_buff) == FALSE || \

			/* peform key exchange base on the algorithm negociated */
			ssh_key_exchange(sshb, &kex_buff) == FALSE) {

		/* an error occured, disconnect */
		clear_buffer(&kex_buff);
		ssh_disconnect(sshb, SSH_DISCONNECT_KEY_EXCHANGE_FAILED, "Unknown error occurs.", 21);

		/* stop cleanly the current process */
		ssh_stop_userproc(sshb, PROC_DIED);
		send(sshb->in_proc, PROC_DIED); // twice in case the version exchange failed.
		return SYSERR;
	}
	clear_buffer(&kex_buff);

	/* Take key into use */
	ssh_send_msg(sshb, SSH_MSG_NEWKEYS);

	/* wait for confirmation */
	if (recvtime(SSH_NEWKEY_TIMEOUT) != NEW_KEY_RECEIVED) {
		ssh_disconnect(sshb, SSH_DISCONNECT_KEY_EXCHANGE_FAILED, "Timeout for NEWKEY", 18);
		ssh_stop_userproc(sshb, PROC_DIED);
		return SYSERR;
	}

	ssh_stop_userproc(sshb, NEW_KEY_SENT);
	return OK;
}

/*-------------------------------------------------------------------------------------
 * nextsshid - gives the next available ssh entry or SYSERR if there are none.
 *-------------------------------------------------------------------------------------
 */
sshid32 nextsshid() {
	static sshid32 nextsshid = 0;
	uint32 i;

	for (i = 0 ; i < SSH_BLOCKS ; ++i) {
		if (nextsshid == SSH_BLOCKS)
			nextsshid = 0;
		if (sshtab[nextsshid].state == SSH_FREE) {
			return nextsshid++;
		} else {
			nextsshid++;
		}
	}
	return (sshid32) SYSERR;
}
