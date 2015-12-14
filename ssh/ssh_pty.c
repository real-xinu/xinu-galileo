#include <xinu.h>

void ssh_ptyout(struct ssh_ptyarg* args, byte* buf, int32 length) {

    byte                buffer[1024];
    byte*               wt;
    struct sshchannel*  sshc;

    sshc = &((args->sshb)->channels[args->channel]);
    if (wait(sshc->sem) == SYSERR)
        return;


    if (sshc->max_pcksize < length || 700 < length) {
        kprintf("%d < %d || 700 < %d\n", sshc->max_pcksize, length, length);
        panic("TODO: ssh_ptyout");
    }

    /* check the windows */
    if (sshc->win_size_out < length) {
        panic("TODO: ssh_ptyout");
    }

    sshc->win_size_out -= length;
    signal(sshc->sem);

    wt = ((struct ssh_binary_packet*)buffer)->buff;

    *wt++ = SSH_MSG_CHANNEL_DATA;

    *((uint32 *) wt) = (uint32) htonl(sshc->s_channel);
    wt += 4;

    length = ssh_write_string(wt, (char *)buf, length);

    ssh_tlp_out(args->sshb, (struct ssh_binary_packet*)buffer, length + 5);

    return;
}

void ssh_ptyin(did32 pty, byte* buff, int32 length) {
    struct	dentry*         devptr;	/* Address of device control blk*/
    struct	ptycblk*        pyptr;		/* Pointer to ptytab entry	*/

    devptr = (struct dentry *) &devtab[pty];
    pyptr = &ptytab[ devptr->dvminor ];

    while (length-- > 0) {
        ptyhandle_in(pyptr, *buff++);
    }
}

did32 ssh_allocate_pty(struct ssh_ptyarg* args) {
    control(PCONSOLE, PC_OUTPUT_FUNC, (int32)ssh_ptyout, (int32)args);
    return PCONSOLE;
}

