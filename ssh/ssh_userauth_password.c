#include <xinu.h>

void generate_hash(byte* salt, char* username, int32 usize, char* password, int32 psize, byte* diggest) {
    struct hash_state state;

    sha1_init(&state);
    sha1_update(&state, salt, SALT_SIZE);
    sha1_update(&state, (byte *)username, usize);
    sha1_update(&state, (byte *)password, psize);
    sha1_finish(&state, diggest);
}


bool8 change_password(char* username, char* oldpw, char* password) {

    /* generate hash */
    // generate_hash(pwtab[userid].salt, password, pwtab[userid].pwhash);
    return FALSE;
}

bool8 check_password(struct pwent* pw, char* username, int32 usize, char* password, int32 psize) {
    byte diggest[SHA1_DIGGEST_SIZE];

    /* generate hash */
    generate_hash(pw->salt, username, usize, password, psize, diggest);

    return (memcmp(diggest, pw->pwhash, SHA1_DIGGEST_SIZE) == 0 ? TRUE : FALSE);
}

/* This function verifes the validy of the password stored in a binary packet. */
/* nbytes is the offset of data read so far to check username and service. */
bool8 ssh_password_check(struct sshent* sshb, struct ssh_binary_packet* in, int32 nbytes, struct pwent* pw) {

    byte* wt;
    char password[MAX_PASSWORD_LENGTH];
    int32 psize;

    wt = in->buff + nbytes;

    /* verify that false should be here */
    if (*wt++ != 0) {
        freebuf((char *)in);
        ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid packet format (1)", 25);
        return FALSE;
    }
    nbytes++;

    /* read password (copy maximun MAX_PASSWORD_LENGTH) */
    psize = ssh_read_string(wt, password, MAX_PASSWORD_LENGTH);
    nbytes += psize - in->packet_length;
    psize -= 4;
    psize = psize > MAX_PASSWORD_LENGTH ? MAX_PASSWORD_LENGTH : psize;

    freebuf((char *)in);

    /* Verify packet format was correct */
    if (nbytes != 0) {
        memset(password, 0, MAX_PASSWORD_LENGTH);
        ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid packet format (2)", 25);
        return FALSE;
    }

    return check_password(pw, sshb->username, sshb->usize, password, psize);
}
