#include <xinu.h>

int ssh_clean_port(int32 buf) {
	freebuf((char *) buf);
	return OK;
}

/*-------------------------------------------------------------------------------------
 * ssh_clean_entry - reset all values in the sshent
 *-------------------------------------------------------------------------------------
 */
void ssh_clean_entry(struct sshent* sshb) {
	int32 j;

	wait(Ssh.sshmutex);

	/* set block FREE */
	sshb->state = SSH_FREE;

	/* delete semaphore */
	semdelete(sshb->sem_ent);
	sshb->sem_ent = SYSERR;

	/* close port */
	ptdelete(sshb->port, ssh_clean_port);
	sshb->port = SYSERR;

	sshb->in_proc = SYSERR;
	sshb->user_proc = SYSERR;
	sshb->service_act = NONE;

	sshb->seq_out = 0;
	sshb->seq_in = 0;

	sshb->kexid = NONE;
	sshb->sid_length = 0;

	sshb->pkid = NONE;

	memset(sshb->enckeyblob_in, 0, ciphtab[CIPHER(sshb->ciphid_in)].keyblob_size);
	memset(sshb->enckeyblob_out, 0, ciphtab[CIPHER(sshb->ciphid_out)].keyblob_size);
	freemem((char *)sshb->enckeyblob_in, ciphtab[CIPHER(sshb->ciphid_in)].keyblob_size);
	freemem((char *)sshb->enckeyblob_out, ciphtab[CIPHER(sshb->ciphid_out)].keyblob_size);
	sshb->enckeyblob_out = (byte *)SYSERR;
	sshb->enckeyblob_in = (byte *)SYSERR;

	sshb->ciphid_out = NONE;
	sshb->ciphid_in = NONE;
	sshb->macid_out = NONE;
	sshb->macid_in = NONE;

	sshb->comid_out = NONE;
	sshb->comid_in = NONE;

	memset(sshb->session_id, 0, MAX_HASH_DIGGEST);
	memset(sshb->iv_in, 0, MAX_KEYS_SIZE);
	memset(sshb->iv_out, 0, MAX_KEYS_SIZE);
	memset(sshb->mackey_in, 0, MAX_KEYS_SIZE);
	memset(sshb->mackey_out, 0, MAX_KEYS_SIZE);

	/* user authentication */
	memset(sshb->username, 0, MAX_NAME_LENGTH);
	sshb->usize = 0;
	memset(sshb->auth_methods, 0, sizeof(int32) * NB_AUTH_METHODS);

	/* connection */
	for (j = 0 ; j < NB_CHANNELS ; j++) {
		sshb->channels[j].state = CHANNEL_FREE;
		sshb->channels[j].program = NONE;
		sshb->channels[j].ppid = SYSERR;
		semdelete(sshb->channels[j].sem);
		sshb->channels[j].sem = SYSERR;
		sshb->channels[j].pty = SYSERR;
		sshb->channels[j].s_channel = SYSERR;
		sshb->channels[j].win_size_in = 0;
		sshb->channels[j].win_size_out = 0;
		sshb->channels[j].max_pcksize = 0;
	}

	signal(Ssh.sshmutex);
}

/*-------------------------------------------------------------------------------------
 * ssh_disconnect - send disconnect message and close a SSH connexion.
 *-------------------------------------------------------------------------------------
 */
void ssh_disconnect(struct sshent* sshb, uint32 code, char* msg, int32 length) {

	byte buff[MAX_DISCONNECT_BUFFER_LENGTH];
	struct ssh_binary_packet* bpck = (struct ssh_binary_packet *) buff;
	byte* wt = bpck->buff;

	*wt++ = SSH_MSG_DISCONNECT;
	*((uint32 *)wt) = htonl(code);
	wt += 4;

	if (length > MAX_DISCONNECT_MSG_LENGTH)
		length = MAX_DISCONNECT_MSG_LENGTH;

	length = ssh_write_string(wt, msg, length);

	ssh_tlp_out(sshb, bpck, 5 + length);

	/* close */
	ssh_close(sshb);

	return;
}

void ssh_stop_userproc(struct sshent* sshb, umsg32 msg) {

	wait(sshb->sem_ent);

	/* set current service to NONE */
	sshb->service_act = NONE;

	/* delete user pid value */
	sshb->user_proc = SYSERR;

	signal(sshb->sem_ent);

	/* tell the in_proc this process is going to die */
	while (send(sshb->in_proc, msg) == SYSERR) {
		sleepms(5);
	}

	return;
}

/*-------------------------------------------------------------------------------------
 * ssh_close - close a SSH connexion
 *-------------------------------------------------------------------------------------
 */
void ssh_close(struct sshent* sshb) {

	wait(sshb->sem_ent);
	if (sshb->state == SSH_CLOSING || sshb->state == SSH_FREE) {
		signal(sshb->sem_ent);
		return;
	}

	/* close tcp connection */
	tcp_close(sshb->tcp_id);
	sshb->tcp_id = SYSERR;

	/* don't free the block yet, proc_in will do it */
	sshb->state = SSH_CLOSING;

	/* wake up user process (if waiting for incoming packet) */
	ptsend(sshb->port, SYSERR);

	signal(sshb->sem_ent);

	return;
}
