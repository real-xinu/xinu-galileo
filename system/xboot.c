/*  xboot.c  - xboot main function code, loads a Xinu image from the boot server and executes it */

#include <xinu.h>

#define XBOOT_SIGNATURE "Xinu"
#define XBOOT_VERSION   0

#pragma pack(2)
struct xboot_header {
	char xboot_signature1[4];      /* xboot signature - Xinu */
	uint32 xboot_header_version;   /* xboot header version - 0 */
	uint32 xboot_load_address;     /* Address to load Xinu */
	uint32 xboot_branch_address;   /* Address to branch to after loading Xinu */
	uint32 xboot_file_size;        /* Xinu image size (does not include xboot header) */
	uint32 xinu_crc32;             /* CRC32 checksum of the Xinu image */
	char xboot_signature2[4];      /* xboot signature - Xinu */
	char xboot_pad[484];           /* xboot header pad - should be all zeros */
};
#pragma pack()

uint32 save_eax = 1;
uint32 save_ebx = 1;
uint32 save_ecx = 1;
uint32 save_edx = 1;
uint32 save_esp = 1;
uint32 save_ebp = 1;
uint32 save_esi = 1;
uint32 save_edi = 1;
uint32 save_eflags = 1;

extern uint32 bootsign;	           /* Boot signature of the boot loader */
extern struct multiboot_bootinfo *bootinfo;  /* Base address of the multiboot info */

extern void xbootjmp(void*);

/* Values for computing CRC32 used to checksum the Xinu image */
uint32 crc_table[] = {
	0x4DBDF21C, 0x500AE278, 0x76D3D2D4, 0x6B64C2B0,
	0x3B61B38C, 0x26D6A3E8, 0x000F9344, 0x1DB88320,
	0xA005713C, 0xBDB26158, 0x9B6B51F4, 0x86DC4190,
	0xD6D930AC, 0xCB6E20C8, 0xEDB71064, 0xF0000000
};

/*------------------------------------------------------------------------
 * crc32 - Computes the CRC32 checksum of the given data
 *------------------------------------------------------------------------
 */
uint32 crc32(byte* data, uint32 data_size) 
{
	uint32 i, crc = 0;

	for (i = 0; i < data_size; i++) {
		crc = (crc >> 4) ^ crc_table[(crc ^ (data[i] >> 0)) & 0x0F];  /* lower nibble */
		crc = (crc >> 4) ^ crc_table[(crc ^ (data[i] >> 4)) & 0x0F];  /* upper nibble */
	}
	
	return crc;
}

/*------------------------------------------------------------------------
 * validate_xinuheader - Validates the xboot header
 *   - Ensures the header signatures are correct
 *   - Ensures the header version is correct
 *   - Ensures the header contains the correct pad
 *------------------------------------------------------------------------
 */
status validate_xinuheader(const struct xboot_header* boot_hdr) 
{
	int i;

	/* Sanity checking on xboot header signature*/
	if(memcmp((void*)boot_hdr->xboot_signature1, (void*)XBOOT_SIGNATURE, sizeof(boot_hdr->xboot_signature1)) != 0) {
		kprintf("[XBOOT] Invalid Xinu header signature: Bad signature 1\n");
		return SYSERR;
	}
	if(memcmp((void*)boot_hdr->xboot_signature2, (void*)XBOOT_SIGNATURE, sizeof(boot_hdr->xboot_signature2)) != 0) {
		kprintf("[XBOOT] Invalid Xinu header signature: Bad signature 2\n");
		return SYSERR;
	}
	
	/* Sanity checking on xboot pad */
	for(i = 0; i < sizeof(boot_hdr->xboot_pad); i++) {
		if(boot_hdr->xboot_pad[i] != 0) {
			kprintf("[XBOOT] Invalid Xinu header signature: Pad not NULL %d\n", i);
			return SYSERR;
		}
	}
	
	/* Sanity checking on header version */
	if(boot_hdr->xboot_header_version != XBOOT_VERSION) {
		kprintf("[XBOOT] Xinu header image not valid: %d\n", boot_hdr->xboot_header_version);
		return SYSERR;
	}

	return OK;
}

/*------------------------------------------------------------------------
 * validate_xinu_loadaddress - Validates the xboot load address
 *   - Ensures that the load address is in a usable block
 *------------------------------------------------------------------------
 */
status validate_xinu_loadaddress(uint32 load_address, uint32 image_size)
{
	uint32	*mmap_addr;	    /* ptr to mmap entries          */
	uint32	*mmap_addrend;  /* ptr to end of mmap region    */
	uint32	mmap_length;    /* length of entire mmap region	*/
	uint32	bootflags = 0;  /* boot information flags       */

	/* The Xinu image must fit in contiguous memory */
	/*   Using the memory map passed by GRUB ensure */
	/*   that the image will fit                    */
	
	if( bootsign == MULTIBOOT_SIGNATURE ) {
		bootflags = bootinfo->flags;
	}

	if((bootflags & MULTIBOOT_BOOTINFO_MMAP) == 0) {
		kprintf("[XBOOT] Unable to load Xinu Image -- No Memory Map\n");
		return SYSERR;
	}

	/* Get the address of the base of mmap region passed by GRUB */
	mmap_addr = (uint32 *)bootinfo->mmap_addr;

	/* Get the length of the mmap region passed by GRUB */
	mmap_length = bootinfo->mmap_length;

	/* Calculate the address end of the byte after the mmap block */
	mmap_addrend = mmap_addr + (mmap_length / 4);
	
	while(mmap_addr < mmap_addrend) {

		uint32	addr;   /* Base address of mmap block	*/
		uint32	len;    /* Length in bytes of mmap block*/
		uint32	type;   /* Type of the mmap block - Type = 1 is usable RAM block */

		addr = *(mmap_addr + 1); /* Base address of block is at offset 1*/
		len  = *(mmap_addr + 3); /* Length of block is at offset 3	*/
		type = *(mmap_addr + 5); /* Type of block is at offset 5	*/

		/* Check if the block of memory is usable */
		if(type != 1 && len > 0) { /* not usable check if address is in Xinu load space */
			if(addr >= load_address	&& addr <= (load_address + image_size - 1)) {
				kprintf("[XBOOT] Invalid Xinu load address %08X -- Address in protected memory\n", load_address);
				return SYSERR;
			}
		}
		
		/* Go to the next mmap block */
		mmap_addr = (uint32 *)((uint32)mmap_addr + (*mmap_addr) + 4);
	}
	
	return OK;	
}

/*------------------------------------------------------------------------
 * xboot - Xinu boot main function 
 *    - Obtains an IP address over DHCP and retrieves a boot image from
 *      the boot server
 *------------------------------------------------------------------------
 */
int32 main(void)
{
	uint32  boot_server_ip;       /* IP address of boot server */
	char    boot_file_name[128];  /* Path to the boot file */
	uint32  boot_file_size = sizeof(boot_file_name);
	
	struct  xboot_header boot_hdr; /* Xboot header on the Xinu image */
	uint32  size;                  /* Size downloaded from TFTP server */
	char*   tftp_buffers[2];       /* Pointers to buffers used for downloading the Xinu image */
	uint32  tftp_buffer_sizes[2];  /* Buffer sizes for TFTP buffers */ 

	kprintf("\n\n###########################################################\n\n");
			
	kprintf("[XBOOT] starting the network...\n");
	kprintf("[XBOOT] initializing network stack...\n");
	net_init();
	
	/* Delay because Ethernet driver doesn't work without it */
	sleepms(800);
	
	/* Force system to use DHCP to obtain an address */
	kprintf("[XBOOT] using dhcp to obtain an IP address...\n");
	uint32 ipaddr = getlocalip_boot(&boot_server_ip, boot_file_name, &boot_file_size);
	if (ipaddr == SYSERR) {
		panic("[XBOOT] Error: could not obtain an IP address\n\r");
	}
	kprintf("\n[XBOOT] IP address is %d.%d.%d.%d   (0x%08x)\n\r",
		(ipaddr>>24)&0xff, (ipaddr>>16)&0xff, (ipaddr>>8)&0xff,
		ipaddr&0xff,ipaddr);
	
	/* Retrieve the xboot header from the boot server */
	kprintf("[XBOOT] Retrieving Xinu Boot Header...\n");
	size = tftpget(boot_server_ip, boot_file_name, (char*)&boot_hdr, sizeof(struct xboot_header), TFTP_NON_VERBOSE);
	if(size == SYSERR) {
		kprintf("[XBOOT] Unable to load Xinu boot header from boot server\n");
		return SYSERR;
	}
	
	kprintf("[XBOOT] Xinu Boot Info:\n[XBOOT] Load address: 0x%08X\n[XBOOT] Branch Address: 0x%08X\n[XBOOT] Image Size: 0x%08X\n", boot_hdr.xboot_load_address, boot_hdr.xboot_branch_address, boot_hdr.xboot_file_size);
	
	/* Validate the Xinu header (xboot header and load address) */
	if(OK != validate_xinuheader(&boot_hdr)) {
		kprintf("[XBOOT] Error: xboot header is not valid\n");
		return SYSERR;
	}
	if(OK != validate_xinu_loadaddress(boot_hdr.xboot_load_address, boot_hdr.xboot_file_size)) {
		return SYSERR;
	}
	
	/* Retrieve the Xinu image from the boot server */
	kprintf("[XBOOT] Loading Xinu...\n");
	tftp_buffers[0] = (char*)&boot_hdr;
	tftp_buffer_sizes[0] = sizeof(struct xboot_header);
	
	tftp_buffers[1] = (char*)boot_hdr.xboot_load_address;
	tftp_buffer_sizes[1] = boot_hdr.xboot_file_size;
	
	size = tftpget_mb(boot_server_ip, boot_file_name, tftp_buffers, tftp_buffer_sizes, 2, TFTP_VERBOSE);
	if(size == SYSERR) {
		kprintf("[XBOOT] Error: unable to load Xinu from boot server\n");
		return SYSERR;
	}
	
	kprintf("[XBOOT] Xinu loaded, validating image...\n");
	uint32 checksum = crc32((byte*)boot_hdr.xboot_load_address, boot_hdr.xboot_file_size);
	if(checksum != boot_hdr.xinu_crc32) {
		kprintf("[XBOOT] Error: Xinu image is not valid - checksum failure %08X != %08X\n", checksum, boot_hdr.xinu_crc32);
		return SYSERR;
	}
	kprintf("[XBOOT] Xinu image checksum: %08X\n", checksum);
	kprintf("[XBOOT] Branching to Xinu image...\n");
	
	xbootjmp((void*)boot_hdr.xboot_branch_address);
	
	/* xbootjmp should not return but if it does, print an error message to the user */
	kprintf("[XBOOT] Error: branch to Xinu image failed\n");
	return SYSERR;
}
