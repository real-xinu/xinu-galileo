#include <xinu.h>

/* Alphabetic order */
static char* requests[NB_REQUESTS] = {"", "pty-req", "shell"};

struct ssh_ptyarg args[NB_SSHPTY];

void ssh_global_request_handler(struct sshent* sshb, struct ssh_binary_packet* in) {
    char name[128];
    int32 length;

    length = ssh_read_string(in->buff + 1, name, 127);
    printf("Received GLOBAL request: %s\n", name);
    /* If answer wanted, send failure */
    if (in->buff[length + 2]) {
        ssh_send_msg(sshb, SSH_MSG_REQUEST_FAILURE);
    }
    freebuf((char *)in);
    return;
}

int32 requests_id(const char* name, int32 length) {
    int32 i = 1;
    int32 diff = -1;

    while (i < NB_REQUESTS && (diff = strncmp(requests[i], name, length)) < 0 ) {
        i++;
    }

    return diff == 0 ? i : NONE;
}

bool8 is_valid_request(int32 cstate, int32 rid) {

    if (cstate == CHANNEL_SESSION && (rid == CHANNEL_PTY || rid == CHANNEL_SHELL))
        return TRUE;

    return FALSE;
}

void parse_ptyreq(byte* buff) {
    int32 length;
    byte op;
    int32 c;

    length = (int32) ntohl(*((uint32 *)buff));
    xdump_array(buff + 4, length);
    buff += length + 4;

    kprintf("Terminal width: %d\n", (int32) ntohl(*((uint32 *)buff)));
    buff += 4;
    kprintf("Terminal height: %d\n", (int32) ntohl(*((uint32 *)buff)));
    buff += 4;
    kprintf("Terminal wpixels: %d\n", (int32) ntohl(*((uint32 *)buff)));
    buff += 4;
    kprintf("Terminal hpixels: %d\n", (int32) ntohl(*((uint32 *)buff)));
    buff += 4;


    length = (int32) ntohl(*((uint32 *)buff));
    buff += 4;
    for (; length > 0 ; length -= 5) {
        op = *buff++;
        c = (int32) ntohl(*((uint32 *)buff));
        buff += 4;
        kprintf("%d: %4x\n", op & 0xFF, c);
    }
}

void ssh_channel_request_handler(struct sshent* sshb, struct ssh_binary_packet* in) {

    char name[REQUEST_MAXLEN];
    int32 length, rid;
    byte* wt;
    uint32 rchannel;
    bool8 want_reply;
    struct sshchannel* sshc;

    wt = in->buff + 1;
    /* Extract channel id */
    rchannel = (uint32) ntohl(*((uint32 *) wt));
    wt += 4;

    /* Extract requet */
    length = ssh_read_string(wt, name, REQUEST_MAXLEN);
    rid = requests_id(name, length - 4);
    wt += length;

    /* want reply ? */
    want_reply = *wt++;

    /* Verify channel valid and open */
    /* Verify request valid */
    wait(sshb->sem_ent);
    if (!IS_VALID_CHANNEL(rchannel) || sshb->channels[rchannel].state == CHANNEL_FREE
     || !is_valid_request(sshb->channels[rchannel].state, rid)) {
        signal(sshb->sem_ent);

        kprintf("Receive invalid request:\n");
        xdump_array(in->buff, in->packet_length);
        if (want_reply)
            ssh_send_msg(sshb, SSH_MSG_REQUEST_FAILURE);
        freebuf((char *) in);
        return;
    }

    sshc = &(sshb->channels[rchannel]);

    if (wait(sshc->sem) == SYSERR) {
        signal(sshb->sem_ent);
        return;
    }
    signal(sshb->sem_ent);

    switch (rid) {
        case CHANNEL_SHELL:
            if (in->packet_length != (int32) (wt - in->buff)) {
                signal(sshc->sem);
                freebuf((char *) in);
                ssh_disconnect(sshb, SSH_DISCONNECT_PROTOCOL_ERROR, "Invalid packet length", 21);
                return;
            }
            freebuf((char *) in);

            if (sshc->pty != SYSERR && sshc->program == NONE) {
                ssh_createshell(sshb, sshc);
                signal(sshc->sem);
                if (want_reply)
                    ssh_send_msg(sshb, SSH_MSG_REQUEST_SUCCESS);
                return;
            }
            signal(sshc->sem);

            if (want_reply)
                ssh_send_msg(sshb, SSH_MSG_REQUEST_FAILURE);
            break;
        case CHANNEL_PTY:

            /* Mutual exclusion needed here */
            args[0].sshb = sshb;
            args[0].channel = rchannel;
            sshc->pty = ssh_allocate_pty(&(args[0]));
            signal(sshc->sem);

            if (want_reply)
                ssh_send_msg(sshb, SSH_MSG_REQUEST_SUCCESS);

            // TODO: read pty request
            // parse_ptyreq(wt);

            freebuf((char *)in);
            break;
    }

    return;
}

void ssh_channel_request_send_controlflow(struct sshent* sshb, struct sshchannel* sshc, bool8 onoff) {

    byte    buffer[MAX_BUFFER_LENGTH(4 + 4 + 8 + 1 + 1)];
    byte*   wt;

    wt = ((struct ssh_binary_packet* )buffer)->buff;

    *wt++ = SSH_MSG_CHANNEL_REQUEST;

    *((uint32 *) wt) = (uint32) htonl(sshc->s_channel);
    wt += 4;

    wt += ssh_write_string(wt, "xon-xoff", 8);

    *wt++ = 0;
    *wt++ = onoff & 0x1;

    ssh_tlp_out(sshb, (struct ssh_binary_packet* )buffer, 18);
}
