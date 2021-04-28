/*  main.c  - main */

#include <xinu.h>
extern float rfs_cache_test(
    uint32 seq_or_rand, /* 0 does sequential reading, > 0 does random reading */
    uint32 cache_size,  /* number of blocks allowed in cache */
    uint32 file_size,   /* size of file to read from */
    uint32 num_reads,   /* number of read operations */
    uint32 check_reads, /* 0 to not check if reads are correct, > 0 does check */
	uint32 quiet); 		/* 0 prints debug messages, > 0 does not */

process	main(void)
{
    
    	kprintf("\nHello World!\n");
    	kprintf("\nI'm the first XINU app and running function main() in system/main.c.\n");
    	kprintf("\nI was created by nulluser() in system/initialize.c using create().\n");
    	kprintf("\nMy creator will turn itself into the do-nothing null process.\n");
    	kprintf("\nI will create a second XINU app that runs shell() in shell/shell.c as an example.\n");
    	kprintf("\nYou can do something else, or do nothing; it's completely up to you.\n");
    	kprintf("\n...creating a shell\n");

	/* Run the Xinu shell */
	recvclr();
	kprintf("Hello Anderson David!\n");
	
	float total_time;
	uint32 filesize = 262144;
	uint32 num_reads = 2048;
	uint32 seq_or_rand = 0;

	// total_time = 0.0;
	// for (int trial = 0; trial < 5; trial++) {
	// 	total_time += rfs_cache_test(seq_or_rand, 0, filesize, num_reads, 0, 1);
	// }

	// kprintf("Average time without caching: %f seconds\n", total_time / 5.0);

	for (int cache_size = 1; cache_size <= 256; cache_size *= 2) {
		total_time = 0.0;
		for (int trial = 0; trial < 5; trial++) {
			total_time += rfs_cache_test(seq_or_rand, cache_size, filesize, num_reads, 0, 1);
		}
		kprintf("Average time for cache size = %d: %f seconds\n", cache_size, total_time / 5.0);
	}

	// total_time = 0.0;
	// for (int trial = 0; trial < 5; trial++) {
	// 	total_time += rfs_cache_test(seq_or_rand, 8, filesize, num_reads, 0, 1);
	// }
	// kprintf("Average time for cache size = %d: %f seconds\n", 8, total_time / 5.0);

	/* Wait for shell to exit and recreate it */

	// while (TRUE) {
	// 	receive();
	// 	sleepms(200);
	// 	kprintf("\n\nMain process recreating shell\n\n");
	// 	resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	// }
	return OK;
    
}
