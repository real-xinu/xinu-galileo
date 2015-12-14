#include <xinu.h>

//void close_channels(struct sshent* sshb) {
//    int32 i;
//
//    for (i = 0 ; i < NB_CHANNELS ; i++) {
//        close_channel(sshb, &(sshb->channels[i]));
//    }
//}


bool8 ssh_connection(struct sshent* sshb) {
    struct ssh_binary_packet* in;
    bool8 run;

    run = TRUE;

    while (run) {
        if ((in = (struct ssh_binary_packet*) ptrecv(sshb->port)) == (struct ssh_binary_packet*)SYSERR) {
            return FALSE;
        }

        switch (in->buff[0]) {
            case SSH_MSG_CHANNEL_OPEN:
                ssh_channel_open_handler(sshb, in);
                break;
            case SSH_MSG_CHANNEL_DATA:
                ssh_channel_data_handler(sshb, in);
                break;
            case SSH_MSG_GLOBAL_REQUEST:
                ssh_global_request_handler(sshb, in);
                break;
            case SSH_MSG_CHANNEL_REQUEST:
                ssh_channel_request_handler(sshb, in);
                break;
            case SSH_MSG_CHANNEL_WINDOW_ADJUST:
                ssh_channel_windowadjust(sshb, in);
                break;
            case SSH_MSG_CHANNEL_CLOSE:
                ssh_channel_close(sshb, in);
                break;
            default:
                kprintf("Message %d, %d bytes\n", in->buff[0], in->packet_length);
                // xdump_array(in->buff, in->packet_length);
                freebuf((char *) in);
                break;
        }
    }

    return FALSE;
}
