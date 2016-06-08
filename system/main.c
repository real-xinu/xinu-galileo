/*  main.c  - main */

#include <xinu.h>
#include <math.h>

void prod2(sid32, sid32);
void cons2(sid32, sid32);
int32 n = 0;
			

process	main(void)
{   
  /* In an effort to make testing of the pthreads interface easier, this function has been filled with various examples
   * that make use of Xinu's multi-process functionality. As development on the pthreads interface progresses, corresponding
   * examples that make use of the pthreads interface will be added here to allow for direct comparison.   */

  /*
  kprintf("+---------------------------------------------+\n");
  kprintf("|       STARTING NATIVE XINU TEST CASES       |\n");
  kprintf("+---------------------------------------------+\n");
  */

  //Example from Section 2.8 of OS Design Vol. 1: "Producer and Consumer processes synchronized with semaphores"
  sid32 produced, consumed;
  consumed = semcreate(0);
  produced = semcreate(1);
  resume(create(cons2, 1024, 20, "cons", 2, consumed, produced));
  resume(create(prod2, 1024, 20, "prod", 2, consumed, produced));

  /*
  kprintf("+---------------------------------------------+\n");
  kprintf("|      DONE WITH NATIVE XINU TEST CASES       |\n");
  kprintf("|  MOVING ON TO PTHREAD INTERFACE TEST CASES  |\n");
  kprintf("+---------------------------------------------|\n");
  */
    
}

void prod2(sid32 consumed, sid32 produced) {
  int32 i;
  for(i = 1; i <= 10; i++) {
    wait(consumed);
    n++;
    signal(produced);
  }
}

void cons2(sid32 consumed, sid32 produced) {
  int32 i;
  for(i = 1; i <= 10; i++) {
    wait(produced);
    kprintf("n is %d\n", n);
    signal(consumed);
  }
}
