#include <xinu.h>

// sort in alphabetic order but "none" stays at the index 0
char* auth_methods[NB_AUTH_METHODS] = {"none", "password", "publickey", "hostbased"};

#define CHECK_TIMEOUT(start, time, delay) ((start) <= (time) ? (start) + (delay) < (time) : (start) < (time) - (delay))

int32 mystrncmp(char* s1, byte* s2, int32 n) {

	while (--n >= 0 && *s1 == *s2) {
		s1++;
		s2++;
	}

	if (*s1 == '\0' && *s2 == '\n')
		return 0;

	return (n < 0 ? 0 : *s1 - *s2);
}

int32 find_user(char* username, byte* buff, int32 buffsize, int32* bread) {
	int32 i, j, diff;

	i = 0;
	j = 0;
	diff = 1;
	while (i < buffsize && buff[i] != '\n' && (diff = mystrncmp(username, buff + i, MAX_NAME_LENGTH)) > 0) {
		j++;
		// Skip a line
		do {
			i++;
		} while (i < buffsize && buff[i - 1] != '\n');
	}

	*bread = i;
	return diff == 0 ? 0x80000000 | j : j;
}

int32 load_file(char* filename, byte** buff, int32 extra) {
	int32 file, fsize;
	byte* tmp;

	if (extra < 0)
		return SYSERR;

	if ((file = open(SSH_FILES, filename, "ro")) == SYSERR) {
#ifdef SSH_DEBUG
		kprintf("load_file: cannot load %s\n", filename);
#endif
		return SYSERR;
	}

	// Read complete file
	if ((fsize = control(SSH_FILES, RFS_CTL_SIZE, file, 0)) == SYSERR ||
			(tmp = (byte *)getmem(fsize + extra)) == (byte *) SYSERR) {
#ifdef SSH_DEBUG
		kprintf("load_file: cannot allocate memory\n");
#endif
		close(file);
		return SYSERR;
	}

	if (read(file, (char *)tmp, fsize) != fsize) {
		freemem((char *) tmp, fsize + extra);
		*buff = (byte *)SYSERR;
		close(file);
#ifdef SSH_DEBUG
		kprintf("load_file: cannot read file\n");
#endif
		return SYSERR;
	}
	close(file);

	*buff = tmp;

	return fsize;
}

int32 write_file(char* filename, bool8 new, byte* buff, int32 size) {
	int32 file;

	if (size < 0)
		return SYSERR;

	if (new) {
		file = open(SSH_FILES, filename, "wn");
	} else {
		file = open(SSH_FILES, filename, "wo");
	}

	if (file == SYSERR) {
		return SYSERR;
	}

	if (write(file, (char *)buff, size) != size) {
		close(file);
		return SYSERR;
	}
	close(file);

	return OK;
}


int32 ssh_userexists(struct sshent* sshb, struct pwent* pw) {

	int32 uid, fsize, i;
	byte* fptr;
	int32 entrysize;
	int32 st;

	/* load password file and look for user */
	if ((fsize = load_file(PASSWD_FILE, &fptr, 0)) == SYSERR) {
		return SYSERR;
	}

	uid = find_user(sshb->username, fptr, fsize, &i);
	freemem((char *)fptr, fsize);

	/* load shadow file */
	if ((fsize = load_file(SHADOW_FILE, &fptr, 0)) == SYSERR) {
		return SYSERR;
	}


	/* is user doesn't exists fill with 0 */
	if (!(uid & 0x80000000)) {
		memset(pw, 0, sizeof(struct pwent));
		for (i = 1 ; i < NB_AUTH_METHODS ; i++) {
			sshb->auth_methods[i] = 1;
		}
		st = NONE;
	} else {
		entrysize = BASE64_ENCODED_SIZE(sizeof(struct pwent)) + 1;
		uid &= 0x7FFFFFFF;
		base64_decode(&(fptr[uid * entrysize]), entrysize - 1, (byte * ) pw);
		sshb->auth_methods[PASSWORD] = 1;
		st = 1;
	}
	freemem((char *)fptr, fsize);

	return st;
}

int32 ssh_authmethod_id(char* method) {

	int32 i = 1;
	int32 diff = -1;

	while (i < NB_AUTH_METHODS && (diff = strncmp(method, auth_methods[i], MAX_AUTH_METHOD_LENGTH)) > 0) {
		i++;
	}

	return diff == 0 ? i : NONE;
}

bool8 ssh_send_msg_userauth_failure(struct sshent* sshb, bool8 psuccess) {
	byte    buff[MAX_BUFFER_LENGTH(sizeof(auth_methods) + 4 * NB_AUTH_METHODS + 1)];
	byte* out = ((struct ssh_binary_packet*) buff)->buff;
	int32 length;

	/* set message id */
	*out++ = SSH_MSG_USERAUTH_FAILURE;

	/* write partial authentication methods list */
	length = ssh_write_list_part(out, auth_methods, sshb->auth_methods, NB_AUTH_METHODS);
	out += length;

	/* partial success ? */
	*out = psuccess & 0x1;

	return ssh_tlp_out(sshb, (struct ssh_binary_packet*) buff, length + 2);
}

/* This method reads the first part of the authentication request               */
/* it extracts the username, the required service and the authentication method */
int32 ssh_read_auth_request(struct sshent* sshb, struct ssh_binary_packet* in, char* user_s, int32* service, int32* auth_method) {
	byte* wt;
	int32 byte_read, tmp;

	char service_s[MAX_SERVICE_LENGTH];
	char auth_method_s[MAX_AUTH_METHOD_LENGTH];

	wt = in->buff;
	byte_read = 1;

	if (*wt++ != SSH_MSG_USERAUTH_REQUEST) {
		return SYSERR;
	}

	/* read user name */
	tmp = ssh_read_string(wt, user_s, MAX_NAME_LENGTH);
	byte_read += tmp;
	wt += tmp;

	/* read service */
	tmp = ssh_read_string(wt, service_s, MAX_SERVICE_LENGTH);
	byte_read += tmp;
	wt += tmp;

	/* read auth_methods */
	byte_read += ssh_read_string(wt, auth_method_s, MAX_AUTH_METHOD_LENGTH);

	/* Extract ids */
	*auth_method = ssh_authmethod_id(auth_method_s);
	*service = ssh_service_id(service_s);

	return byte_read;
}

bool8 ssh_validate_auth_request(struct sshent* sshb, int32 current_user, int32 user, int32 current_service, int32 service) {
	if (current_service == service && current_user == user) {
		return TRUE;
	}

	ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Username or service changed.", 28);
	return FALSE;
}

bool8 ssh_userauth(struct sshent* sshb) {
	struct ssh_binary_packet* in;
	int32 nbytes;

	char username[MAX_NAME_LENGTH];
	int32 userexists;
	struct pwent pw;

	int32 current_serid;
	int32 serid;

	int32 authmid;

	int32 ntries_left = MAX_AUTH_TRIES;
	int32 auth_success = 1;

	/* it is not a hard timeout (can be around 10 sec wrong) */
	/* NOTE: can be a problem if Xinu runs longer than 136 years... :) */
	uint32 start = clktime;

	/* read initial athentication request */
	if ((in = (struct ssh_binary_packet*)ptrecv(sshb->port)) == (struct ssh_binary_packet*)SYSERR) {
		return FALSE;
	}

	if ((nbytes = ssh_read_auth_request(sshb, in, sshb->username, &current_serid, &authmid)) != in->packet_length) {
		ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Expected authentication method list.", 36);
		freebuf((char *)in);
		return FALSE;
	}

	sshb->usize = strnlen(sshb->username, MAX_NAME_LENGTH);

	if ((userexists = ssh_userexists(sshb, &pw)) == SYSERR) {
		ssh_disconnect(sshb, SSH_DISCONNECT_BY_APPLICATION, "Unexpected error.", 17);
		freebuf((char *)in);
		return FALSE;
	}

	/* look for service id */
	if (current_serid == NONE) {
		ssh_disconnect(sshb, SSH_DISCONNECT_SERVICE_NOT_AVAILABLE, "Service not available.", 22);
		freebuf((char *)in);
		return FALSE;
	}

	/* stop if the state becomes IN_CLEAR(means disconnected) */
	wait(sshb->sem_ent);
	while (STATUS(sshb->state) == ESTABLISHED) {
		signal(sshb->sem_ent);

		switch (authmid) {
			case PUBKEY:
			case NONE:
				/* send the possible authentication method for this user */
				freebuf((char *)in);
				if (ssh_send_msg_userauth_failure(sshb, FALSE) == FALSE) {
					return FALSE;
				}
				break;
			case PASSWORD:
				if (ssh_password_check(sshb, in, nbytes, &pw) == TRUE) {
					/* one less authentication needed */
					auth_success--;
				}  else {
					if (--ntries_left > 0) {
						ssh_send_msg_userauth_failure(sshb, FALSE);
					} else {
						ssh_disconnect(sshb, SSH_DISCONNECT_NO_MORE_AUTH_METHODS_AVAILABLE, "No more authentication methods available", 40);
						return FALSE;
					}
				}
				break;
			default:
				panic("Should not happen");
				break;
		}

		/* enought authentication method have succeed */
		if (userexists == 1 && auth_success == 0) {
			wait(sshb->sem_ent);
			break;
		}

		/* If timeout didn't expire and no messages in the list sleep for a little while */
		while (!CHECK_TIMEOUT(start, clktime, AUTH_TIMEOUT_TOT_SEC) && ptcount(sshb->port) == 0) {
			sleepms(AUTH_TIMEOUT_LAP_MS);
		}

		/* If timeout disconnect */
		if (CHECK_TIMEOUT(start, clktime, AUTH_TIMEOUT_TOT_SEC)) {
			ssh_disconnect(sshb, SSH_DISCONNECT_BY_APPLICATION, "User authentication timeout.", 28);
			return FALSE;
		}

		/* At this point a message is in the list, only this process can access it */
		if ((in = (struct ssh_binary_packet*) ptrecv(sshb->port)) == (struct ssh_binary_packet*)SYSERR) {
			return FALSE;
		}

		/* read new request */
		if ((nbytes = ssh_read_auth_request(sshb, in, username, &serid, &authmid)) == SYSERR) {
			ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Expected Authentication request.", 32);
			freebuf((char *)in);
			return FALSE;
		}

		if (current_serid != serid || strncmp(sshb->username, username, MAX_NAME_LENGTH) != 0) {
			ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Username or service changed.", 28);
			return FALSE;
		}

		wait(sshb->sem_ent);
	}

	if (STATUS(sshb->state) == ESTABLISHED) {
		sshb->state = SET_STATUS(sshb->state, AUTHENTICATED);
		sshb->service_act = current_serid;
		signal(sshb->sem_ent);
		ssh_send_msg(sshb, SSH_MSG_USERAUTH_SUCCESS);
		return TRUE;
	} else {
		signal(sshb->sem_ent);
		return FALSE;
	}
}
