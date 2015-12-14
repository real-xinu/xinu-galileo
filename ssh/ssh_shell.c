#include <xinu.h>

process ssh_shell(struct sshent* sshb, struct sshchannel* sshc) {

    shell(sshc->pty);

    if (wait(sshc->sem) == SYSERR) {
        return OK;
    }
    sshc->ppid = SYSERR;
    signal(sshc->sem);

    close_channel(sshb, sshc);
    return OK;
}

void ssh_createshell(struct sshent* sshb, struct sshchannel* sshc) {

    struct procent* prptr;  /* Pointer to proc. table entry */

    sshc->ppid = create(ssh_shell, SSH_SHELL_STACK_SIZE, SSH_SHELL_PRIO, "ssh_shell", 2, sshb, sshc, NULL);

    prptr = &proctab[sshc->ppid];
    prptr->prdesc[0] = sshc->pty;
    prptr->prdesc[1] = sshc->pty;
    prptr->prdesc[2] = sshc->pty;

    sshc->program = CHANNEL_SHELL;
    resume(sshc->ppid);
    return;
}
