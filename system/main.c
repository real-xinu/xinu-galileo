/*  main.c  - main */

#include <xinu.h>
extern struct tcpcontrol Tcp; /* TCP Control			*/

process main(void) {

    /* Run the Xinu shell */

    recvclr();
    //Ad hoc TCP Test
    kprintf("PR 500 - Next Port: %d\n\n", Tcp.tcpnextport);
    int32 slot;
    int i, j;

    /* Connect to 128.10.136.101:50000 */
    uint32 serverip;
    //128.10.136.60
    //128.10.135.31


    char string[58400];

    for (j = 0; j < 5; j++) {
        kprintf("Trial: %d\n", j + 1);

        dot2ip("128.10.136.61", &serverip);
        slot = tcp_register(serverip, 2004, 1);

        tcp_send(slot, string, 58400);

        tcp_close(slot);
    }


    resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

    /* Wait for shell to exit and recreate it */

    while (TRUE) {
        receive();
        sleepms(200);
        kprintf("\n\nMain process recreating shell\n\n");
        resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
    }
    return OK;

}
