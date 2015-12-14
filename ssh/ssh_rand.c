#include <xinu.h>

void ssh_initrand() {

#ifndef SSH_DEBUG
    /* found some randomness */
    init_context(&(Ssh.sshctx));
#else
    byte* seed = (byte *) (Ssh.sshctx).randrsl;
    int32 i;
    for (i = 0 ; i < 32 ; i++) {
        seed[i] = i & 0xFF;
    }
    randinit(&(Ssh.sshctx), TRUE);
#endif

}

void ssh_initnewctx(randctx* ctx, randctx* mstctx) {
    int32 i;
    static int32 rseed = 0;

    if (--rseed <= 0) {
        ssh_initrand();
        rseed = SSH_RESEED;
    }

    for (i = 0 ; i < RANDSIZL ; i++) {
        ctx->randrsl[i] = crand(mstctx);
    }

    randinit(ctx, TRUE);
}
