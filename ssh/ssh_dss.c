#include <xinu.h>

int32 read_dsscert(byte *buff, struct dsacert* certificate) {
    int32 length, wt, nbytes;
    char name[8];

    /* read length */
    length = (int32)ntohl(*((uint32 *)buff));
    buff += 4;

    /* Verify it is a dss-cert */
    wt = ssh_read_string(buff, name, 8);
    if (wt != (4 + 7) || \
            strncmp(name, "ssh-dss", 7) != 0) {
#ifdef SSH_DEBUG
        kprintf("read_dsscert: invalid initial string: %s\n", name);
#endif
        return SYSERR;
    }
    buff += wt;

    nbytes = ssh_read_mpint(buff, &((certificate->p).num));
    wt += nbytes;
    if (nbytes == SYSERR || wt > length) {
        return SYSERR;
    }

    buff += nbytes;
    nbytes = ssh_read_mpint(buff, &(certificate->q));
    wt += nbytes;
    if (nbytes == SYSERR || wt > length) {
        clear_mpprime(&(certificate->p));
        return SYSERR;
    }
    buff += nbytes;
    nbytes = ssh_read_mpint(buff, &(certificate->g));
    wt += nbytes;
    if (nbytes == SYSERR || wt > length) {
        clear_mpprime(&(certificate->p));
        clear_mpint(&(certificate->q));
        return SYSERR;
    }

    buff += nbytes;
    nbytes = ssh_read_mpint(buff, &(certificate->y));
    wt += nbytes;
    if (nbytes == SYSERR || wt != length) {
        clear_mpprime(&(certificate->p));
        clear_mpint(&(certificate->q));
        clear_mpint(&(certificate->g));
        return SYSERR;
    }

    return length + 4;
}

int32 write_dsscert(byte *buff, struct dsacert* certificate) {
    uint32* length = (uint32 *)buff;
    int32 nbytes;

    buff += 4;
    buff += ssh_write_string(buff, "ssh-dss", 7);
    buff += ssh_write_mpint(buff, &((certificate->p).num));
    buff += ssh_write_mpint(buff, &(certificate->q));
    buff += ssh_write_mpint(buff, &(certificate->g));
    buff += ssh_write_mpint(buff, &(certificate->y));

    nbytes = (int32) (buff - (byte *)length - 4);
    *length = htonl(nbytes);

    return nbytes + 4;
}

bool8 aux_readdsssign(MPINT* dst, byte* buff) {
    byte* wtdst;
    int32 i;

    if (extend_mpint(dst, 20 / sizeof(HALF_WORD)) == FALSE) {
        return FALSE;
    }

    wtdst = (byte *)dst->val + 19;
    /* copy value */
    for (i = 0 ; i < 20 ; i++) {
        *buff++ = *wtdst--;
    }

    return TRUE;
}

int32 read_dsssign(byte* buff, struct dsasign* sign) {
    char name[8];
    int32 wt;

    if ((uint32)ntohl(*((uint32 *) buff)) != 55) {
        return SYSERR;
    }
    buff += 4;

    /* Verify it is a dss-sign */
    if ((wt = ssh_read_string(buff, name, 8)) != (4 + 7) || \
            strncmp(name, "ssh-dss", 7) != 0) {
        return SYSERR;
    }
    buff += 15;


    if (aux_readdsssign(&(sign->r), buff) == FALSE) {
        return SYSERR;
    }
    buff += 20;

    if (aux_readdsssign(&(sign->s), buff) == FALSE) {
        clear_mpint(&(sign->r));
        return SYSERR;
    }

    return 59;
}

void aux_writedsssign(byte* buff, MPINT* src) {

    byte* wtsrc = (byte *)src->val;
    int32 i;

    buff += 19;
    /* copy value */
    for (i = 0 ; i < 20 ; i++) {
        *buff-- = *wtsrc++;
    }
}

int32 write_dsssign(byte* buff, struct dsasign* sign) {

    /* write length */
    *((uint32 *) buff) = (uint32)htonl(55);
    buff += 4;

    /* write dss-sign */
    buff += ssh_write_string(buff, "ssh-dss", 7);
    *((uint32 *) buff) = (uint32)htonl(40);
    buff += 4;
    aux_writedsssign(buff, &(sign->r));
    buff += 20;
    aux_writedsssign(buff, &(sign->s));

    return 59;
}
