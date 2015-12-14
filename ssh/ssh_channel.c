#include <xinu.h>

/* IN ALPHABETIC ORDER */
char* channels[NB_CHANNEL_TYPES] = {"", "session"};

int32 channel_type_id(char* ctype, int32 length) {
	int32 i = 1;
	int32 diff = -1;

	while (i < NB_CHANNEL_TYPES && (diff = strncmp(channels[i], ctype, length)) < 0 ) {
		i++;
	}

	return diff == 0 ? i : NONE;
}

uint32 new_channel(struct sshent* sshb) {

	int32 i = 0;

	while (i < NB_CHANNELS && sshb->channels[i].state != CHANNEL_FREE) {
		i++;
	}

	return i < NB_CHANNELS ? i : SYSERR;
}

void ssh_channel_open_confirmation(struct sshent* sshb, int32 ctype, uint32 rchannel, uint32 schannel) {

	byte buff[MAX_BUFFER_LENGTH(17)];
	struct ssh_binary_packet* bpck = (struct ssh_binary_packet *) buff;
	byte* wt = bpck->buff;

	*wt++ = SSH_MSG_CHANNEL_OPEN_CONFIRMATION;

	*((uint32 *) wt) = (uint32) htonl(rchannel);
	wt += 4;

	*((uint32 *) wt) = (uint32) htonl(schannel);
	wt += 4;

	*((uint32 *) wt) = (uint32) htonl(CHANNEL_INIT_WINSIZE);
	wt += 4;

	*((uint32 *) wt) = (uint32) htonl(CHANNEL_MAX_MSGSIZE);
	wt += 4;

	ssh_tlp_out(sshb, bpck, 17);
}

void ssh_channel_open_failure(struct sshent* sshb, uint32 schannel, uint32 code, const char* msg, int32 length) {

	byte buff[128];
	struct ssh_binary_packet* bpck = (struct ssh_binary_packet *) buff;
	byte* wt = bpck->buff;

	*wt++ = SSH_MSG_CHANNEL_OPEN_FAILURE;
	*((uint32 *) wt) = (uint32) htonl(schannel);
	wt += 4;
	*((uint32 *) wt) = (uint32) htonl(code);
	wt += 4;
	wt += ssh_write_string(wt, msg, length);
	wt += ssh_write_string(wt, "", 0);

	ssh_tlp_out(sshb, bpck, (int32) (wt - bpck->buff));
}

void ssh_channel_open_handler(struct sshent* sshb, struct ssh_binary_packet* in) {

	byte* wt;
	char ctype[CHANNEL_TYPE_MAXLEN];
	int32 clength;
	uint32 schannel, iwsize, mpck, ch;
	struct  sshchannel* sshc;

	wait(sshb->sem_ent);
	if (STATUS(sshb->state) != AUTHENTICATED) {
		signal(sshb->sem_ent);
		freebuf((char *)in);
		ssh_disconnect(sshb, SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT, "No user authenticated", 21);
		return;
	}
	signal(sshb->sem_ent);

	/* validate incoming message */
	wt = in->buff + 1;

	/* read channel type */
	clength = ssh_read_string(wt, ctype, CHANNEL_TYPE_MAXLEN);
	wt += clength;

	/* read sender channel */
	schannel = (uint32) ntohl(*((uint32 *) wt));
	wt += 4;

	/* initial windows size */
	iwsize = (uint32) ntohl(*((uint32 *) wt));
	wt += 4;

	/* maximum packet size */
	mpck = (uint32) ntohl(*((uint32 *) wt));
	wt += 4;

	/* channel specific data */
	switch (channel_type_id(ctype, clength)) {
		case CHANNEL_SESSION:
			/* No more data for session */
			if ((int32) (wt - in->buff) != in->packet_length) {
				freebuf((char *)in);
				ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid packet format", 21);
				return;
			}
			freebuf((char *)in);

			wait(sshb->sem_ent);
			if ((ch = new_channel(sshb)) == SYSERR ||
					(sshb->channels[ch].sem = semcreate(1)) == SYSERR) {
				signal(sshb->sem_ent);
				ssh_channel_open_failure(sshb, schannel, SSH_OPEN_RESSOURCE_SHORTAGE, "No more channel available.", 26);
				return;
			}

			sshc = &(sshb->channels[ch]);

			sshc->state = CHANNEL_SESSION;
			sshc->program = NONE;
			sshc->ppid = SYSERR;
			sshc->s_channel = schannel;
			sshc->win_size_in = CHANNEL_INIT_WINSIZE;
			sshc->win_size_out = iwsize;
			sshc->max_pcksize = mpck > MAX_PCKSIZE ? MAX_PCKSIZE : mpck;

			signal(sshb->sem_ent);

			ssh_channel_open_confirmation(sshb, CHANNEL_SESSION, schannel, ch);
			ssh_channel_request_send_controlflow(sshb, sshc, TRUE);
			break;
		default:
			if ((int32) (wt - in->buff) > in->packet_length) {
				freebuf((char *)in);
				ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid packet format", 21);
				return;
			}
			freebuf((char *)in);
			ssh_channel_open_failure(sshb, schannel, SSH_OPEN_UNKNOWN_CHANNEL_TYPE, "Unknown channel type.", 21);
			break;
	}
}

void adjust_window(struct sshent* sshb, struct sshchannel* sshc, uint32 incr) {
	byte    buff[MAX_BUFFER_LENGTH(9)];
	byte*   out = ((struct ssh_binary_packet *) buff)->buff;

	*out++ = SSH_MSG_CHANNEL_WINDOW_ADJUST;

	*((uint32 *) out) = (uint32) htonl(sshc->s_channel);
	out += 4;

	*((uint32 *) out) = (uint32) htonl(incr);

	ssh_tlp_out(sshb, (struct ssh_binary_packet *)buff, 9);

	sshc->win_size_in += incr;
	return;

}

void ssh_channel_data_handler(struct sshent* sshb, struct ssh_binary_packet* in) {

	uint32 channel, length;
	byte* wt;
	struct sshchannel*      sshc;

	wt = in->buff + 1;
	channel = (uint32) ntohl(*((uint32 *) wt));
	wt += 4;

	// Check if channel is valid
	wait(sshb->sem_ent);
	if (!IS_VALID_CHANNEL(channel) || sshb->channels[channel].state <= CHANNEL_FREE) {
		freebuf((char *) in);
		signal(sshb->sem_ent);
		return;
	}

	// Check format
	length = (uint32) ntohl(*((uint32 *) wt));
	wt += 4;

	if (length + 9 != in->packet_length) {
		freebuf((char *) in);
		signal(sshb->sem_ent);
		ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid format.", 15);
		return;
	}

	sshc = &(sshb->channels[channel]);

	// Check and update window
	if (wait(sshc->sem) == SYSERR) {
		freebuf((char *) in);
		signal(sshb->sem_ent);
		return;
	}
	signal(sshb->sem_ent);

	if (sshc->win_size_in < length)
		panic("data_handler: TODO");

	sshc->win_size_in -= length;
	if (sshc->win_size_in < CHANNEL_WINSIZE_THRES) {
		adjust_window(sshb, sshc, CHANNEL_WINSIZE_ADD);
	}

	switch (sshc->program) {
		case CHANNEL_SHELL:
			signal(sshc->sem);
			ssh_ptyin(sshc->pty, wt, length);
			break;
		default:
			panic("(data_handler) Shouldn't happen");
	}
	freebuf((char *) in);

}

void ssh_channel_windowadjust(struct sshent* sshb, struct ssh_binary_packet* in) {

	byte*               wt;
	uint32              channel, incr;
	struct sshchannel*  sshc;

	if (in->packet_length != 9) {
		ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid format.", 15);
		freebuf((char *) in);
		return;
	}

	wt = in->buff + 1;

	channel = (uint32) ntohl(*((uint32 *)wt));
	wt += 4;

	wait(sshb->sem_ent);
	if (!IS_VALID_CHANNEL(channel) || sshb->channels[channel].state <= CHANNEL_FREE) {
		signal(sshb->sem_ent);
		freebuf((char *) in);
		return;
	}

	sshc = &(sshb->channels[channel]);
	incr = (uint32) ntohl(*((uint32 *)wt));
	freebuf((char *) in);

	if (wait(sshc->sem) == SYSERR) {
		signal(sshb->sem_ent);
		return;
	}
	signal(sshb->sem_ent);

	if (sshb->channels[channel].win_size_out + incr <= sshb->channels[channel].win_size_out) {
		signal(sshc->sem);
		return;
	}

	sshb->channels[channel].win_size_out += incr;
	signal(sshc->sem);

}

void close_program(struct sshchannel* sshc) {

	int32 k;

	switch (sshc->program) {
		case CHANNEL_SHELL:
			if (sshc->ppid == SYSERR)
				return;

			// HACKKKKK
			ssh_ptyin(sshc->ppid, (byte *)"\nexit\n", 6);

			k = 0;
			while (k++ < 10 && wait(sshc->sem) != SYSERR && sshc->ppid != SYSERR) {
				signal(sshc->sem);
				sleepms(10);
			}

			/* Shell didn't terminate, kill it... */
			if (sshc->ppid != SYSERR)
				kill(sshc->ppid);
			signal(sshc->sem);
			break;
	}
}

void close_channel(struct sshent* sshb, struct sshchannel* sshc) {

	byte    buff[MAX_BUFFER_LENGTH(4)];
	byte*   wt;

	wait(sshb->sem_ent);
	if (sshc->state <= CHANNEL_FREE ||
			wait(sshc->sem) == SYSERR) {
		signal(sshb->sem_ent);
		return;
	}
	sshc->state = CHANNEL_CLOSING;
	signal(sshb->sem_ent);

	close_program(sshc);
	sshc->program = NONE;
	sshc->ppid = SYSERR;

	if (sshc->pty != SYSERR) {
		control(sshc->pty, PC_RESET, 0, 0);
	}
	sshc->pty = SYSERR;

	sshc->win_size_in = 0;
	sshc->win_size_out = 0;
	sshc->max_pcksize = 0;
	semdelete(sshc->sem);
	sshc->sem = SYSERR;

	wt = ((struct ssh_binary_packet*)buff)->buff;

	*wt++ = SSH_MSG_CHANNEL_CLOSE;
	*((uint32 *)wt) = (uint32) htonl(sshc->s_channel);

	ssh_tlp_out(sshb, (struct ssh_binary_packet*)buff, 5);

	wait(sshb->sem_ent);
	sshc->state = CHANNEL_FREE;
	sshc->s_channel = SYSERR;
	signal(sshb->sem_ent);
}

void ssh_channel_close(struct sshent* sshb, struct ssh_binary_packet* in) {

	byte*               wt;
	uint32              channel;
	struct sshchannel*  sshc;

	if (in->packet_length != 5) {
		freebuf((char *) in);
		ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid format.", 15);
		return;
	}
	wt = in->buff + 1;

	channel = (uint32) ntohl(*((uint32 *)wt));
	freebuf((char *) in);

	wait(sshb->sem_ent);
	if (!IS_VALID_CHANNEL(channel) || sshb->channels[channel].state <= CHANNEL_FREE) {
		signal(sshb->sem_ent);
		return;
	}

	sshc = &(sshb->channels[channel]);
	signal(sshb->sem_ent);

	close_channel(sshb, sshc);
}
