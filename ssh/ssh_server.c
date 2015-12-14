#include <xinu.h>

bool8 send_to_userproc(struct sshent* sshb, struct ssh_binary_packet* buff) {
	if (ptsend(sshb->port, (umsg32)buff) == SYSERR) {
		freebuf((char *) buff);
		return FALSE;
	}

	return TRUE;
}

process ssh_server(int32 tcp_id) {

	sshid32 sshid;
	struct sshent* sshb;
	struct ssh_binary_packet* in;
	byte msg_type;
	int32 nbfail = 0;

#ifdef SSH_DEBUG
	printf("ssh_server: start server %d\n", tcp_id);
#endif

	recvclr();

	/* Initialize entry and launch user process */
	if ((sshid = ssh_accept(tcp_id)) == SYSERR) {
		return SYSERR;
	}

	sshb = &(sshtab[sshid]);

	/* Wait for the user process to exchange version (it doesn't use the binary packet protocol) */
	receive();

	/* Receive incoming packet and act if necessary (DISCONNECT, IGNORE etc.)  */
	/* Or send a message to the user process with the buffer used */
	wait(sshb->sem_ent);
	while (sshb->state != SSH_CLOSING) {
		signal(sshb->sem_ent);


		in = (struct ssh_binary_packet*) getbuf(Ssh.bpin);

		if (ssh_tlp_in(sshb, in) == TRUE) {
			nbfail = 0;
			msg_type = in->buff[0];
			switch(msg_type) {
				/* Forward to USER process */
				/* Temporary */
				case SSH_MSG_KEXINIT:
				case SSH_MSG_KEXDH_INIT:
					wait(sshb->sem_ent);
					if (STATUS(sshb->state) > IN_CLEAR) {
						signal(sshb->sem_ent);
						ssh_disconnect(sshb, SSH_DISCONNECT_BY_APPLICATION, "Re-keying is not supported", 26);
						freebuf((char *) in);
						break;
					}
					signal(sshb->sem_ent);

				case SSH_MSG_USERAUTH_REQUEST:
				case SSH_MSG_CHANNEL_OPEN:
				case SSH_MSG_CHANNEL_REQUEST:
				case SSH_MSG_CHANNEL_WINDOW_ADJUST:
				case SSH_MSG_CHANNEL_DATA:
				case SSH_MSG_CHANNEL_EXTENDED_DATA:
				case SSH_MSG_CHANNEL_CLOSE:
				case SSH_MSG_CHANNEL_EOF:
					if (!send_to_userproc(sshb, in)) {
						ssh_disconnect(sshb, SSH_DISCONNECT_BY_APPLICATION, "Application doesn't answer", 26);
					}
					break;
				case SSH_MSG_NEWKEYS:
					ssh_msg_newkeys(sshb, in);
					break;
				case SSH_MSG_SERVICE_REQUEST:
					ssh_msg_service_request(sshb, in);
					break;
				case SSH_MSG_DEBUG:
					ssh_msg_debug(in);
					break;
				case SSH_MSG_DISCONNECT:
					ssh_msg_disconnect(sshb, in);
					break;
				default:
					ssh_disconnect(sshb, SSH_DISCONNECT_BY_APPLICATION, "", 0);
				case SSH_MSG_UNIMPLEMENTED:
					// kprintf("get an unimplemented message seq #%d\n", ntohl(*((uint32 *) (in->buff + 1))));
				case SSH_MSG_IGNORE:
					freebuf((char *) in);
					break;

			}
		} else {
			freebuf((char *)in);
			if (++nbfail > 10) {
				send_to_userproc(sshb, (struct ssh_binary_packet *)SYSERR);
				wait(sshb->sem_ent);
				break;
			}
		}

		wait(sshb->sem_ent);
	}
	signal(sshb->sem_ent);

	/* Make sure the user process exit */
	while (receive() != PROC_DIED);

	/* Completly free the block */
	ssh_clean_entry(sshb);

	return OK;
}

process ssh_deamon() {
	int32 tcp_id;
	int32 child;

#ifdef SSH_DEBUG
	kprintf("ssh_deamon: started\n");
#endif

	/* register on port SSH_PORT */
	tcp_id = tcp_register(0, SSH_PORT, 0);
	if (tcp_id == SYSERR) {
#ifdef SSH_DEBUG
		kprintf("ssh_deamon: cannot open connection\n");
#endif
		return SYSERR;
	}

	while (tcp_recv(tcp_id, (char *)&child, 4) != SYSERR) {
#ifdef SSH_DEBUG
		kprintf("ssh_deamon: incomming connection\n");
#endif
		resume(create(ssh_server, SSH_DEAMON_STACK_SIZE, SSH_DEAMON_PRIO, "ssh_server", 1, child));
		receive();
	}

	tcp_close(tcp_id);
#ifdef SSH_DEBUG
	kprintf("ssh_deamon: receive error\n");
#endif
	return SYSERR;
}

