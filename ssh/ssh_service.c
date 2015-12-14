#include <xinu.h>

char* services[NB_SERVICES] = {"none", "ssh-connection", "ssh-userauth"};

int32 ssh_service_id(char* msg) {

    int32 i, diff;

    i = 1;
    while (i < NB_SERVICES  && (diff = strncmp(msg, services[i], MAX_SERVICE_LENGTH)) > 0) {
        i++;
    }

    return diff == 0 ? i : 0;
}

process ssh_service_accept(struct sshent* sshb) {

    bool8 run = TRUE;

    while (run) {
        switch (sshb->service_act) {
            case USER_AUTH:
                ssh_send_msg_userauth_banner(sshb);
                run = ssh_userauth(sshb);
                break;
            case CONNECTION:
                run = ssh_connection(sshb);
                break;
            default:
                /* should not happen */
                ssh_disconnect(sshb, SSH_DISCONNECT_SERVICE_NOT_AVAILABLE, "Service not available.", 22);
                run = FALSE;
                break;
        }
    }

    ssh_stop_userproc(sshb, PROC_DIED);

    return OK;
}
