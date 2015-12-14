#include <xinu.h>

struct sshcontrol Ssh;
struct sshent sshtab[SSH_BLOCKS];

bool8 ssh_init() {

    int32 i, j;

    /* initialize Ssh. */
    Ssh.sshmutex = semcreate(1);
    Ssh.bpin = mkbufpool(SSH_BUFF_SIZE, SSH_BUFF_NB);
    Ssh.bpout = mkbufpool(sizeof(struct ssh_binary_packet), 1);

    if (Ssh.sshmutex == SYSERR || \
            Ssh.bpin == SYSERR || \
            Ssh.bpout == SYSERR) {
#ifdef SSH_DEBUG
        kprintf("ssh_init: fail creating mutex or buffer pools\n");
#endif
        semdelete(Ssh.sshmutex);
        return FALSE;
    }

    for (i = 0 ; i < SSH_BLOCKS ; ++i) {
        sshtab[i].state = SSH_FREE;
        memset(&(sshtab[i].in), 0, sizeof(struct ssh_binary_packet));
        sshtab[i].sem_ent = SYSERR;
        sshtab[i].seq_out = 0;
        sshtab[i].in_proc = SYSERR;
        sshtab[i].user_proc = SYSERR;
        sshtab[i].port = SYSERR;
        sshtab[i].service_act = NONE;
        sshtab[i].seq_in = 0;
        sshtab[i].kexid = NONE;
        sshtab[i].sid_length = 0;
        sshtab[i].ciphid_in = NONE;
        sshtab[i].ciphid_out = NONE;
        sshtab[i].macid_in = NONE;
        sshtab[i].macid_out = NONE;
        memset(sshtab[i].username, 0, MAX_NAME_LENGTH);
        sshtab[i].usize = 0;
        memset(sshtab[i].auth_methods, 0, sizeof(int32) * NB_AUTH_METHODS);

        for (j = 0 ; j < NB_CHANNELS ; j++) {
            sshtab[i].channels[j].state = CHANNEL_FREE;
            sshtab[i].channels[j].program = NONE;
            sshtab[i].channels[j].sem = SYSERR;
            sshtab[i].channels[j].pty = SYSERR;
            sshtab[i].channels[j].s_channel = SYSERR;
            sshtab[i].channels[j].win_size_in = 0;
            sshtab[i].channels[j].win_size_out = 0;
            sshtab[i].channels[j].max_pcksize = 0;
        }
    }

    /* init DES */
    generate_desip_table();
    generate_desperm_table();

    /* load keys */
    init_dsapriv(&xinu_DSApriv);
    init_dsacert(&xinu_DSAcert);
    if (ssh_load_dsakey(&xinu_DSApriv, &xinu_DSAcert, KEY_FOLDER, DSA_KEY) == FALSE) {
        semdelete(Ssh.sshmutex);
        return FALSE;
    }

    return TRUE;
}
