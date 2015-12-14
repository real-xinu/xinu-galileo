#include <xinu.h>
#include <stdio.h>

/** Algorithms negociation */
/*** Key exchange  Algorithms */
#define NB_KEX	2
char* kex_algorithms[NB_KEX] = {"diffie-hellman-group1-sha1", "diffie-hellman-group1-sha1-96"};

/*** Public Key Encryption Algorithms */
#define NB_PKE	1
char* pke_algorithms[NB_PKE] = {"ssh-dss"};

/*** Encryption Algorithms */
#define NB_ENC	2
char* encryption_algorithms[NB_ENC] = {"3des-cbc","none"};

/*** MAC Algorithms */
#define NB_MAC	2
char* mac_algorithms[NB_MAC] = {"hmac-sha1", "hmac-sha1-96"};

/*** Compression Algorithms */
#define NB_CMP	1
char* compression_algorithms[NB_CMP] = {"none"};

bool8 choose_algorithms(struct sshent* sshb, byte* cli_pck, int32 cli_len, byte* ser_pck, int32 ser_len);
bool8 choose_algorithm(struct sshent* sshb, int32 type, byte** cli_pck, int32* cli_len, byte** ser_pck, int32* ser_len);

/*-------------------------------------------------------------------------------------
 * ssh_algorithms_negociation - perform the ssh algorithms negociation
 *-------------------------------------------------------------------------------------
 */
bool8 ssh_algorithms_negociation(struct sshent *sshb, struct buffer *kex_buff) {

    struct ssh_binary_packet out;
    byte *buff = out.buff;
    uint32* len;
    int32 nbytes_s;
    struct ssh_binary_packet* in;
    bool8 st;

    /* Header */
    *buff++ = SSH_MSG_KEXINIT;

    /* Cookie */
    len = (uint32 *)buff;

    wait(sshb->sem_ent);

    *len++ = crand(&(sshb->randctx));
    *len++ = crand(&(sshb->randctx));
    *len++ = crand(&(sshb->randctx));
    *len++ = crand(&(sshb->randctx));

    signal(sshb->sem_ent);

    buff += 4*sizeof(uint32);


    /* Add kex algo */
    buff += ssh_write_list(buff, kex_algorithms, NB_KEX);

    /* Add pke algo */
    buff += ssh_write_list(buff, pke_algorithms, NB_PKE);

    /* Add enc algo */
    /* server -> client */
    buff += ssh_write_list(buff, encryption_algorithms, NB_ENC);

    /* client -> server */
    buff += ssh_write_list(buff, encryption_algorithms, NB_ENC);

    /* Add MAC algo */
    /* server -> client */
    buff += ssh_write_list(buff, mac_algorithms, NB_MAC);

    /* client -> server */
    buff += ssh_write_list(buff, mac_algorithms, NB_MAC);

    /* Add CMP algo */
    /* server -> client */
    buff += ssh_write_list(buff, compression_algorithms, NB_CMP);

    /* client -> server */
    buff += ssh_write_list(buff, compression_algorithms, NB_CMP);

    /* Add language */
    buff += ssh_write_list(buff, NULL, 0);
    buff += ssh_write_list(buff, NULL, 0);

    /* Message following */
    *(buff++) = 0x0;

    /* 0 for the end. */
    *((uint32 *) buff) = 0;
    buff += 4;

    nbytes_s = (int32) (buff - out.buff);
    if (ssh_tlp_out(sshb, &out, nbytes_s) == FALSE) {
#ifdef SSH_DEBUG
        kprintf("\nalgorithms_negociation: Fail sending algorthm list\n");
#endif
        return FALSE;
    }

    /* wait for the other side list. */
    if ((in = (struct ssh_binary_packet *)ptrecv(sshb->port)) == (struct ssh_binary_packet *)SYSERR) {
        return FALSE;
    }

    /* get mutex */
    wait(sshb->sem_ent);

    /* Determine the correct algorithms and copy data to the kex_buffer. */
    st = TRUE;
    switch (SIDE(sshb->state)) {
        case SSH_CLIENT:
            signal(sshb->sem_ent);
            if (choose_algorithms(sshb, out.buff, nbytes_s, in->buff, in->packet_length) == FALSE || \
                    addint_buff(kex_buff, nbytes_s) == FALSE || \
                    append_buff(kex_buff, out.buff, nbytes_s) == FALSE || \
                    addint_buff(kex_buff, in->packet_length) == FALSE || \
                    append_buff(kex_buff, in->buff, in->packet_length) == FALSE) {
                st =  FALSE;
            }
            break;
        case SSH_SERVER:
            signal(sshb->sem_ent);
            if (choose_algorithms(sshb, in->buff, in->packet_length, out.buff, nbytes_s) == FALSE || \
                    addint_buff(kex_buff, in->packet_length) == FALSE || \
                    append_buff(kex_buff, in->buff, in->packet_length) == FALSE || \
                    addint_buff(kex_buff, nbytes_s) == FALSE || \
                    append_buff(kex_buff, out.buff, nbytes_s) == FALSE) {
                st = FALSE;
            }
            break;
        default:
            signal(sshb->sem_ent);
            st = FALSE;
            break;
    }

#ifdef SSH_DEBUG
    kprintf("\nalgorithms_negociation: kex_buff\n");
    xdump_array(kex_buff->buff, (int32)(kex_buff->current - kex_buff->buff));
#endif

    /* done with the buffer */
    freebuf((char *)in);

    return st;
}

bool8 choose_algorithm(struct sshent* sshb, int32 type, byte** cli_pck, int32* cli_len, byte** ser_pck, int32* ser_len) {

    int32 s, c, spc, match;
    byte *wtcli, *wtser;
    bool8 st;

    /* save pointers */
    wtcli = *cli_pck;
    wtser = *ser_pck;

    /* get length of each list */
    int32 clil = (int32)ntohl(*((uint32 *) wtcli));
    int32 serl = (int32)ntohl(*((uint32 *) wtser));

    if (clil > *cli_len || serl > *ser_len) {
        return FALSE;
    }

    wtcli += 4;
    wtser += 4;

    /* find a match between the lists */
    /* go through the client list */
    c = spc = 0;
    match = -1;
    while (spc < clil) {
        s = 0;

        while (s < serl) {
            /* reset position in client list */
            c = spc;

            /* try to match one algo */
            while (s < serl && c < clil && wtser[s] != ',' && wtser[s] == wtcli[c]) {
                s++;
                c++;
            }

            /* algo match, great */
            if ((s == serl || wtser[s] == ',') && (c == clil || wtcli[c] == ',')) {
                match = spc;
                spc = clil; // break outer loop
                break;
            }

            /* no match */
            /* skip the current algo in server list */
            while (s < serl && wtser[s] != ',') {
                s++;
            }
            s++; // skip the ','
        }

        /* First client algo doesn't match */
        /* skip the current algo in client list */
        while (spc < clil && wtcli[spc] != ',') {
            spc++;
        }
        spc++; // skip the ','
    }

    /* No match found */
    if (match ==  -1) {
#ifdef SSH_DEBUG
        kprintf("choose_algorithm: for %d no match found!\n", type);
#endif
        return FALSE;
    }

    /* parse the algorithm base on its type */
    st = TRUE;
    switch (type) {
        case KEX_ALGO:
            if ((match = parse_kex_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else {
                sshb->kexid = match;
            }
            break;

        case SER_HOST_ALGO:
            if ((match = parse_pke_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else {
                sshb->pkid = match;
            }
            break;

        case ENC_ALGO_CS:
            if ((match = parse_enc_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else if (SIDE(sshb->state) == SSH_CLIENT) {
                sshb->ciphid_out = match;
            } else if (SIDE(sshb->state) == SSH_SERVER) {
                sshb->ciphid_in = match;
            } else {
                st = FALSE;
            }
            break;

        case ENC_ALGO_SC:
            if ((match = parse_enc_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else if (SIDE(sshb->state) == SSH_CLIENT) {
                sshb->ciphid_in = match;
            } else if (SIDE(sshb->state) == SSH_SERVER) {
                sshb->ciphid_out = match;
            } else {
                st = FALSE;
            }
            break;

        case MAC_ALGO_CS:
            if ((match = parse_mac_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else if (SIDE(sshb->state) == SSH_CLIENT) {
                sshb->macid_out = match;
            } else if (SIDE(sshb->state) == SSH_SERVER) {
                sshb->macid_in = match;
            } else {
                st = FALSE;
            }
            break;

        case MAC_ALGO_SC:
            if ((match = parse_mac_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else if (SIDE(sshb->state) == SSH_CLIENT) {
                sshb->macid_in = match;
            } else if (SIDE(sshb->state) == SSH_SERVER) {
                sshb->macid_out = match;
            } else {
                st = FALSE;
            }
            break;

        case COM_ALGO_CS:
            if ((match = parse_com_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else if (SIDE(sshb->state) == SSH_CLIENT) {
                sshb->comid_out = match;
            } else if (SIDE(sshb->state) == SSH_SERVER) {
                sshb->comid_in = match;
            } else {
                st = FALSE;
            }
            break;

        case COM_ALGO_SC:
            if ((match = parse_com_algo((char *) &(wtcli[match]), c - match)) == SYSERR) {
                st = FALSE;
            } else if (SIDE(sshb->state) == SSH_CLIENT) {
                sshb->comid_out = match;
            } else if (SIDE(sshb->state) == SSH_SERVER) {
                sshb->comid_in = match;
            } else {
                st = FALSE;
            }
            break;

        default:
            st = FALSE;
    }

    if (st == TRUE) {

        /* update the pointer for the next algo */
        *cli_pck += clil + 4;
        *cli_len -= clil + 4;
        *ser_pck += serl + 4;
        *ser_len -= serl + 4;

        return TRUE;
    }

#ifdef SSH_DEBUG
    kprintf("choose_algorithm: %d type unknown\n", type);
#endif
    return FALSE;

}

bool8 choose_algorithms(struct sshent* sshb, byte* cli_pck, int32 cli_len, byte* ser_pck, int32 ser_len) {

    /* Skip header + cookie */
    cli_pck += 17;
    ser_pck += 17;
    cli_len -= 17;
    ser_len -= 17;

    /* choose each algorithms */
    if (wait(sshb->sem_ent) == SYSERR) {
        return FALSE;
    }

    if (choose_algorithm(sshb, KEX_ALGO     , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, SER_HOST_ALGO, &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, ENC_ALGO_CS  , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, ENC_ALGO_SC  , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, MAC_ALGO_CS  , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, MAC_ALGO_SC  , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, COM_ALGO_CS  , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE || \
            choose_algorithm(sshb, COM_ALGO_SC  , &cli_pck, &cli_len, &ser_pck, &ser_len) == FALSE
       ) {
        signal(sshb->sem_ent);
        return FALSE;
    }

    /* verify other part of the message */
    if ((SIDE(sshb->state) == SSH_CLIENT && (ser_len != 8 + 1 + 4 || *(ser_pck + 9) != 0)) || \
            (SIDE(sshb->state) == SSH_SERVER && (cli_len != 8 + 1 + 4 || *(cli_pck + 9) != 0))) {
#ifdef SSH_DEBUG
        kprintf("\nchoose_algorithms: Language was not empty or guessed pack set by other side\n");
        kprintf("client byte remaining %d, server bytes remaining %d\n", ser_len, cli_len);
        xdump_array(cli_pck, cli_len);
        kprintf("\n");
        xdump_array(ser_pck, ser_len);
#endif
        signal(sshb->sem_ent);
        return FALSE;
    }
    signal(sshb->sem_ent);

    return TRUE;
}
