#include <xinu.h>

void ssh_msg_debug(struct ssh_binary_packet* in) {

	int32 slength;
	if (in->buff[1] != 0) {
		slength = *((uint32 *) (in->buff + 2));
		slength = ntohl(slength);
		if (slength + 5 < BUFFERS_LENGTH) {
			in->buff[slength + 4] = '\0';
			printf("MSG_DEBUG: %s\n", in->buff + 6);
		}
	}
	freebuf((char *) in);
}

void ssh_msg_newkeys(struct sshent* sshb, struct ssh_binary_packet* in) {

	umsg32 msg = 0;
	int32 length;

	length = in->packet_length;
	freebuf((char *)in);
	/* check packet format */
	if (length > 1) {
		ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Incorrect length.", 17);
		return;
	}

	while (send(sshb->user_proc, NEW_KEY_RECEIVED) == SYSERR);

	/* wait for the user process to notify us */
	while (((msg = receive()) >> 16) != PREFIX_PROC_MSG);

	if (msg != NEW_KEY_SENT) {
		send(getpid(), msg);
		return;
	}

	wait(sshb->sem_ent);
	sshb->state = SET_STATUS(sshb->state, ESTABLISHED);
	signal(sshb->sem_ent);
	return;
}

void ssh_msg_service_request(struct sshent* sshb, struct ssh_binary_packet* in) {

	int32   id;
	char    service[MAX_SERVICE_LENGTH];
	byte*   wt;
	byte    buff[MAX_BUFFER_LENGTH(4 + MAX_SERVICE_LENGTH)];
	byte*   out = ((struct ssh_binary_packet*) buff)->buff;
	int32   length;

	wt = in->buff + 1;
	if (ssh_read_string(wt, service, MAX_SERVICE_LENGTH) + 1 != in->packet_length) {
		ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Missing data", 12);
		freebuf((char *) in);
		return;
	}
	freebuf((char *) in);

	/* validate request */
	if (sshb->service_act != NONE || (id = ssh_service_id(service)) == NONE || id != USER_AUTH) {
		ssh_disconnect(sshb, SSH_DISCONNECT_SERVICE_NOT_AVAILABLE, "Service not available.", 22);
		return;
	}

	/* send confirmation */
	length = in->packet_length - 5; // 1 byte for id, 4 bytes for string length

	*out++ = SSH_MSG_SERVICE_ACCEPT;

	if ((length = ssh_write_string(out, service, length)) == SYSERR || \
			ssh_tlp_out(sshb, (struct ssh_binary_packet*) buff, length + 1) == FALSE) {
		return;
	}

	/* start service process */
	sshb->service_act = id;
	sshb->user_proc = create(ssh_service_accept, SSH_SERVER_STACK_SIZE, SSH_SERVER_USER_PRIO, "ssh_service_accept", 1, sshb, NULL);
	resume(sshb->user_proc);

	return;
}

void ssh_msg_disconnect(struct sshent* sshb, struct ssh_binary_packet* in) {
	freebuf((char *) in);
	ssh_close(sshb);  // prevent the user process to do anything else
	return;
}

void ssh_send_msg_userauth_banner(struct sshent* sshb) {

	int32 length = 0;
	static char banner[] = " __  __    ____ ____  _   _ \r\n \\ \\/ /   / ___/ ___|| | | |\r\n  \\  /____\\___ \\___ \\| |_| |\r\n  /  \\_____|__) |__) |  _  |\r\n /_/\\_\\   |____/____/|_| |_|\r\n                            \r\n    Welcome on Xinu!!    \r\n\r\n";
	byte buff[MAX_BUFFER_LENGTH(4 + sizeof(banner) + 4)];
	byte* out = ((struct ssh_binary_packet*) buff)->buff;

	*out++ = SSH_MSG_USERAUTH_BANNER;
	length = ssh_write_string(out, banner, sizeof(banner));
	out += length;
	length += ssh_write_string(out, NULL, 0);

	ssh_tlp_out(sshb, (struct ssh_binary_packet*) buff, length + 1);
}

void ssh_send_msg(struct sshent* sshb, int32 msg_type) {
	byte buff[MAX_MSG_LENGTH];

	((struct ssh_binary_packet*) buff)->buff[0] = msg_type;

	if (ssh_tlp_out(sshb, (struct ssh_binary_packet*)buff, 1) == FALSE) {
		ssh_disconnect(sshb, SSH_DISCONNECT_CONNECTION_LOST, NULL, 0);
	}
}
