/* xsh_sshdump.c - xsh_sshdump */

#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void print_state(int32 state);
void print_kex(int32 kexid);
void print_channel(struct sshchannel* sshc);

/*------------------------------------------------------------------------
 * xsh_sshdump - dump the state of the ssh block
 *------------------------------------------------------------------------
 */
shellcmd xsh_sshdump(int nargs, char *args[]) {

    struct sshent*      sshb;
    struct sshchannel*  sshc;
    int32               i;

    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
        printf("Use: %s [-d ID]\n\n", args[0]);
        printf("Description:\n");
        printf("\tDumps the content of the ssh control blocks\n");
        printf("Options:\n");
        printf("\t-d\t\tdetails the block #ID\n");
        printf("\t--help\t display this help and exit\n");
        return 0;
    }

    if (nargs == 1) {
        printf("SSH blocks statistics:\n");
        printf("\t%d blocks\n", SSH_BLOCKS);
        for (i = 0 ; i < SSH_BLOCKS ; i++) {
            sshb = &sshtab[i];
            printf("\tBlock #%d: ", i);
            print_state(sshb->state);
        }
        return 1;
    }

    if (nargs == 3 && strncmp(args[1], "-d", 2) == 0) {
        i = atoi(args[2]);

        if (i < 0 || i >= SSH_BLOCKS) {
            fprintf(stderr, "%s: invalid argument\n", args[0]);
            fprintf(stderr,
                    "Try '%s --help' for more information\n",
                    args[0]);
            return 0;
        }

        sshb = &sshtab[i];
        printf("\n\tBlock #%d:\n", i);
        switch (STATUS(sshb->state)) {
            case AUTHENTICATED:
                printf("\t\tUser logged: %s\n", sshb->username);
            case ESTABLISHED:
                printf("\t\t%d pck in / %d pck out\n", sshb->seq_in, sshb->seq_out);
                printf("\t\tKey exchange algorithm used: ");
                print_kex(sshb->kexid);
                printf("\t\tCipher in:\t%s-%s\n", ciphtab[CIPHER(sshb->ciphid_in)].name, modetab[MODE(sshb->ciphid_in)].name);
                printf("\t\tMAC in:\t\thmac-%s\n", hashtab[sshb->macid_in].name);
                printf("\t\tCipher out:\t%s-%s\n", ciphtab[CIPHER(sshb->ciphid_out)].name, modetab[MODE(sshb->ciphid_out)].name);
                printf("\t\tMAC out:\thmac-%s\n", hashtab[sshb->macid_out].name);
                break;
            default:
                printf("\t\tNo information\n");
                break;
        }

        for (i = 0 ; i < NB_CHANNELS ; i++) {
            sshc = &sshb->channels[i];
            printf("\n\t\tChannel #%d:\n", i);
            print_channel(sshc);
        }
        return 1;
    }

    fprintf(stderr, "%s: invalid argument\n", args[0]);
    fprintf(stderr,
            "Try '%s --help' for more information\n",
            args[0]);
    return 1;
}

void print_state(int32 state) {
    switch (SIDE(state)) {
        case SSH_FREE:
            printf("FREE\n");
            return;
        case SSH_CLIENT:
            printf("client ");
            break;
        case SSH_SERVER:
            printf("server ");
            break;
        default:
            printf("");
            break;
    }

    switch(STATUS(state)) {
        case SSH_FREE:
            printf(" traffic in clear\n");
            break;
        case ESTABLISHED:
            printf(" connection established\n");
            break;
        case AUTHENTICATED:
            printf(" user authenticated\n");
    }
}

void print_kex(int32 id) {
    switch (GROUP(id)) {
        case DHG1:
            printf("Diffie-Hellman Group1 ");
            break;
        case DHG14:
            printf("Diffie-Hellman Group14 ");
            break;
        default:
            printf("Unknow algorithm ");
            break;
    }

    printf("using the hash %s\n", hashtab[HASH(id)].name);
}

void print_channel(struct sshchannel* sshc) {
    printf("\t\t\tState: ");
    switch (sshc->state) {
        case CHANNEL_FREE:
            printf("FREE\n");
            return;
        case CHANNEL_SESSION:
            printf("interactive session runnning\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }

    printf("\t\t\tProgram: ");
    switch (sshc->program) {
        case CHANNEL_SHELL:
            printf("Shell\n");
            break;
        default:
            printf("Unknown program running\n");
            break;
    }
}
