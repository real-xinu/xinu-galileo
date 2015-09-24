/*  main.c  - main */

#include <xinu.h>

#define DETECTEDDEADLOCK   12
#define NODEADLOCK         10

void tc_p(sid32 sem1, sid32 sem2, pid32 mainproc)
{
    wait(sem1);
    sleepms(500);
    wait(sem2);
    
    signal(sem2);
    signal(sem1);
    
    send(mainproc, NODEADLOCK);
}

void tc_pn(sid32 sem1, sid32 sem2, pid32 mainproc)
{
    wait(sem1);
    sleepms(500);
    if(waitd(sem2) != OK) {
        send(mainproc, DETECTEDDEADLOCK);
    }
    
    signal(sem2);
    signal(sem1);
    
    send(mainproc, NODEADLOCK);
}

/* Test case 1 - original signal and wait should cause deadlock */
void test_case_1(void)
{
    sid32 sem1 = semcreate(1);
    sid32 sem2 = semcreate(1);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_p, 4096, 20, "tc1_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_p, 4096, 20, "tc1_p2", 3, sem2, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 1 Success\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 1 Failed, no deadlock\n");
    } else {
        kprintf("TC 1 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    
    semdelete(sem1);
    semdelete(sem2);
}

/* Test case 2 - original signal and wait should not cause deadlock */
void test_case_2(void)
{
    sid32 sem1 = semcreate(1);
    sid32 sem2 = semcreate(1);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_p, 4096, 20, "tc1_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_p, 4096, 20, "tc1_p2", 3, sem2, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 2 Failed, deadlock\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 2 Success\n");
    } else {
        kprintf("TC 2 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    
    semdelete(sem1);
    semdelete(sem2);
}

/* Test case 3 - original signal and new waitd should cause deadlock and be detected */
void test_case_3(void)
{
    sid32 sem1 = semcreate(1);
    sid32 sem2 = semcreate(1);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_pn, 4096, 20, "tc3_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_pn, 4096, 20, "tc3_p2", 3, sem2, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 3 Failed, deadlock not detected\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 3 Failed, no deadlock\n");
    } else if(msg == DETECTEDDEADLOCK) {
        kprintf("TC 3 Success\n");
    } else {
        kprintf("TC 3 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    
    semdelete(sem1);
    semdelete(sem2);
}

/* Test case 4 - original signal and new waitd should not cause deadlock and should not be detected */
void test_case_4(void)
{
    sid32 sem1 = semcreate(2);
    sid32 sem2 = semcreate(2);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_pn, 4096, 20, "tc4_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_pn, 4096, 20, "tc4_p2", 3, sem2, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 4 Failed, deadlock\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 4 Success\n");
    } else if(msg == DETECTEDDEADLOCK) {
        kprintf("TC 4 Failed, deadlock detected\n");
    } else {
        kprintf("TC 4 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    
    semdelete(sem1);
    semdelete(sem2);
}

/* Test case 5 - original signal and wait should cause deadlock */
void test_case_5(void)
{
    sid32 sem1 = semcreate(1);
    sid32 sem2 = semcreate(1);
    sid32 sem3 = semcreate(1);
    sid32 sem4 = semcreate(1);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_p, 4096, 20, "tc5_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_p, 4096, 20, "tc5_p2", 3, sem2, sem3, getpid());
    pid32 pid3 = create(tc_p, 4096, 20, "tc5_p3", 3, sem3, sem4, getpid());
    pid32 pid4 = create(tc_p, 4096, 20, "tc5_p4", 3, sem4, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resume(pid3);
    resume(pid4);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 5 Success\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 5 Failed, no deadlock\n");
    } else {
        kprintf("TC 5 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    kill(pid3);
    kill(pid4);
    
    semdelete(sem1);
    semdelete(sem2);
    semdelete(sem3);
    semdelete(sem4);
}

/* Test case 6 - original signal and new waitd should cause deadlock and be detected */
void test_case_6(void)
{
    sid32 sem1 = semcreate(1);
    sid32 sem2 = semcreate(1);
    sid32 sem3 = semcreate(1);
    sid32 sem4 = semcreate(1);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_pn, 4096, 20, "tc6_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_pn, 4096, 20, "tc6_p2", 3, sem2, sem3, getpid());
    pid32 pid3 = create(tc_pn, 4096, 20, "tc6_p3", 3, sem3, sem4, getpid());
    pid32 pid4 = create(tc_pn, 4096, 20, "tc6_p4", 3, sem4, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resume(pid3);
    resume(pid4);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 6 Failed, deadlock\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 6 Failed, no deadlock\n");
    } else if(msg == DETECTEDDEADLOCK) {
        kprintf("TC 6 Success\n");
    } else {
        kprintf("TC 6 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    kill(pid3);
    kill(pid4);
    
    semdelete(sem1);
    semdelete(sem2);
    semdelete(sem3);
    semdelete(sem4);
}

/* Test case 7 - original signal and new waitd should not cause deadlock and should not be detected */
void test_case_7(void)
{
    sid32 sem1 = semcreate(2);
    sid32 sem2 = semcreate(2);
    sid32 sem3 = semcreate(2);
    sid32 sem4 = semcreate(2);
    
    recvclr();
    
    resched_cntl(DEFER_START);
    pid32 pid1 = create(tc_pn, 4096, 20, "tc6_p1", 3, sem1, sem2, getpid());
    pid32 pid2 = create(tc_pn, 4096, 20, "tc6_p2", 3, sem2, sem3, getpid());
    pid32 pid3 = create(tc_pn, 4096, 20, "tc6_p3", 3, sem3, sem4, getpid());
    pid32 pid4 = create(tc_pn, 4096, 20, "tc6_p4", 3, sem4, sem1, getpid());
    resume(pid1);
    resume(pid2);
    resume(pid3);
    resume(pid4);
    resched_cntl(DEFER_STOP);
    
    umsg32 msg = recvtime(1000);
    if(msg == TIMEOUT) {
        kprintf("TC 7 Failed, deadlock\n");
    } else if(msg == NODEADLOCK) {
        kprintf("TC 7 Success\n");
    } else if(msg == DETECTEDDEADLOCK) {
        kprintf("TC 7 Failed, deadlock detected\n");
    } else {
        kprintf("TC 7 Failed - unknown message\n");
    }
    
    kill(pid1);
    kill(pid2);
    kill(pid3);
    kill(pid4);
    
    semdelete(sem1);
    semdelete(sem2);
    semdelete(sem3);
    semdelete(sem4);
}

process	main(void)
{
	test_case_1();
    test_case_2();
    test_case_3();
    test_case_4();
    test_case_5();
    test_case_6();
    test_case_7();
	return OK;
}