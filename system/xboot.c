/*  xboot.c  - xboot main function code, loads a Xinu image from the 	*/
/*	boot server and executes it 					*/

#include <xinu.h>

#define XBOOT_SIGNATURE "Xinu"
#define XBOOT_VERSION   0

#pragma pack(2)
struct xboot_hdr {
	char xboot_sig1[4];		/* xboot signature - Xinu 	*/
	uint32 xboot_hdr_ver;		/* xboot header version - 0 	*/
	uint32 xboot_load_addr;		/* Address to load Xinu 	*/
	uint32 xboot_branch_addr; 	/* Address to branch to after 	*/
					/*	loading Xinu 		*/
	uint32 xboot_file_size;		/* Xinu image size (does not 	*/
					/*	include xboot header)	*/
	uint32 xinu_crc32;		/* CRC32 checksum of the Xinu 	*/
					/*	image 			*/
	char xboot_sig2[4];		/* xboot signature - Xinu 	*/
	char xboot_pad[484];		/* xboot header pad - should be	*/
					/*	all zeros 		*/
};
#pragma pack()

extern uint32 bootsign;	           	/* Boot signature of the boot 	*/
					/*	loader 			*/
extern struct mbootinfo *bootinfo; 	/* Base address of the 		*/
					/*	multiboot info 		*/

extern void xbootjmp(void*);

struct  xboot_hdr boot_hdr;		/* Xboot header on the Xinu img	*/
byte	xinu_image_valid;		/* Indicator of valid Xinu img	*/
char*	load_ptr;			/* Ptr to load location		*/

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
uint32 crc32(
	 byte* data, 			/* Data to checksum		*/
	 uint32 data_size		/* Size of data			*/
	)
{
	uint32 i, crc = 0;

	for (i = 0; i < data_size; i++) {
		crc = (crc >> 4) ^ crc_table[(crc ^
			(data[i] >> 0)) & 0x0F];  /* lower nibble */
		crc = (crc >> 4) ^ crc_table[(crc ^
			(data[i] >> 4)) & 0x0F];  /* upper nibble */
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
status validate_xinuheader(
	 const struct xboot_hdr* boot_hdr	/* xboot header		*/
	)
{
	int i;

	/* Sanity checking on xboot header signature*/
	if(memcmp((void*)boot_hdr->xboot_sig1,
		(void*)XBOOT_SIGNATURE,
		sizeof(boot_hdr->xboot_sig1)) != 0) {
		kprintf("[XBOOT] Xinu header signature: Bad sig 1\n");
		return SYSERR;
	}
	if(memcmp((void*)boot_hdr->xboot_sig2,
		(void*)XBOOT_SIGNATURE,
		sizeof(boot_hdr->xboot_sig2)) != 0) {
		kprintf("[XBOOT] Xinu header signature: Bad sig 2\n");
		return SYSERR;
	}
	
	/* Sanity checking on xboot pad */
	for(i = 0; i < sizeof(boot_hdr->xboot_pad); i++) {
		if(boot_hdr->xboot_pad[i] != 0) {
			kprintf("[XBOOT] Xinu Header pad not 0 %d\n", i);
			return SYSERR;
		}
	}
	
	/* Sanity checking on header version */
	if(boot_hdr->xboot_hdr_ver != XBOOT_VERSION) {
		kprintf("[XBOOT] Xinu header image not valid: %d\n",
			boot_hdr->xboot_hdr_ver);
		return SYSERR;
	}

	return OK;
}

/*------------------------------------------------------------------------
 * validate_xinu_loadaddress - Validates the xboot load address
 *   - Ensures that the load address is in a usable block
 *------------------------------------------------------------------------
 */
status validate_xinu_loadaddress(
	 uint32 load_address,	/* Xinu load address			*/
	 uint32 image_size	/* Xinu image size			*/
	)
{
	uint32	*mmap_addr;	/* ptr to mmap entries          	*/
	uint32	*mmap_addrend;	/* ptr to end of mmap region    	*/
	uint32	mmap_length;	/* length of entire mmap region		*/
	uint32	bootflags = 0;	/* boot information flags		*/

	/* The Xinu image must fit in contiguous memory 		*/
	/*   Using the memory map passed by GRUB ensure that the image 	*/
	/*	will fit                    				*/
	
	if( bootsign == MULTIBOOT_SIGNATURE ) {
		bootflags = bootinfo->flags;
	}

	if((bootflags & MULTIBOOT_BOOTINFO_MMAP) == 0) {
		kprintf("[XBOOT] Unable to load Xinu -- No Memory Map\n");
		return SYSERR;
	}

	/* Get the address of the base of mmap region passed by GRUB 	*/
	mmap_addr = (uint32 *)bootinfo->mmap_addr;

	/* Get the length of the mmap region passed by GRUB 		*/
	mmap_length = bootinfo->mmap_length;

	/* Calculate the address end of the byte after the mmap block 	*/
	mmap_addrend = mmap_addr + (mmap_length / 4);
	
	while(mmap_addr < mmap_addrend) {

		uint32	addr;   /* Base address of mmap block		*/
		uint32	len;    /* Length in bytes of mmap block	*/
		uint32	type;   /* Type of the mmap block - Type = 1 	*/
				/*	is usable RAM block 		*/

		addr = *(mmap_addr + 1); 	/* Base address of block*/
						/*	is at offset 1	*/
		len  = *(mmap_addr + 3); 	/* Length of block is at*/
						/*	offset 3	*/
		type = *(mmap_addr + 5); 	/* Type of block is at	*/
						/*	offset 5	*/

		/* Check if the block of memory is usable 		*/
		if(type != 1 && len > 0) { 	/* not usable check if 	*/
						/*	address is in 	*/
						/*	Xinu load space	*/
			if(addr >= load_address	&& addr <= (load_address +
				image_size - 1)) {
				kprintf("[XBOOT] Xinu load address %08X");
				kprintf(" - in protected memory\n",
					load_address);
				return SYSERR;
			}
		}
		
		/* Go to the next mmap block */
		mmap_addr = (uint32 *)((uint32)mmap_addr +
			(*mmap_addr) + 4);
	}
	
	return OK;	
}

/*------------------------------------------------------------------------
 * xboot_cb - TFTP callback function process incomming packets
 *------------------------------------------------------------------------
 */
status xboot_cb(
	  uint16 blk_nbr,	/* TFTP data block number		*/
	  char* blk_ptr,	/* Pointer to TFTP block data		*/
	  uint32 blk_len,	/* Length of TFTP data block		*/
	  byte last_pkt		/* Indicator of last packet		*/
	)
{
	char* src_ptr;		/* Pointer to the packet start		*/ 
	uint32 cpy_len;		/* Amount of data to copy		*/

	/* Validate the Xinu header (xboot header and load address)	*/
	
	if(blk_nbr == 1) {

		xinu_image_valid = FALSE;

		if(blk_len < sizeof(struct xboot_hdr)) {
			kprintf("[XBOOT] Error: Xinu image does not "
				"contain a boot header\n");
			return SYSERR;
		}
	
		memcpy(&boot_hdr, blk_ptr, sizeof(struct xboot_hdr));

		if(OK != validate_xinuheader(&boot_hdr)) {
			kprintf("[XBOOT] Error: xboot header is not "
				"valid\n");
			return SYSERR;
		}
		if(OK != validate_xinu_loadaddress(
			boot_hdr.xboot_load_addr,
			boot_hdr.xboot_file_size)) {
			kprintf("[XBOOT] Error: Xinu load address is "
				"not valid\n");
			return SYSERR;
		}

		load_ptr = (char*)(boot_hdr.xboot_load_addr);
		src_ptr = blk_ptr + sizeof(struct xboot_hdr);
		cpy_len = blk_len - sizeof(struct xboot_hdr);

		xinu_image_valid = TRUE;
	} else {
		src_ptr = blk_ptr;
		cpy_len = blk_len;
	}

	/* Copy the Xinu image data to the load address			*/
	
	if(xinu_image_valid && cpy_len > 0) {
		memcpy(load_ptr, src_ptr, cpy_len);
		load_ptr += cpy_len;
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
	/* Initialize the network stack */
	net_init();
	
	/* Force system to use DHCP to obtain an address */
	uint32 ipaddr = getlocalip();
	if (ipaddr == SYSERR) {
		kprintf("[XBOOT] Could not obtain an IP address\n");
		return SYSERR;
	}

	kprintf("[XBOOT] Loading Xinu...\n");

	/* Assume bad Xinu image until it is verified	*/

	xinu_image_valid = FALSE;
	
	/* Retrieve the xboot header from the boot server */
	if(tftpget(NetData.bootserver, NetData.bootfile,
		(void*)&xboot_cb, TFTP_FUNC_MAGIC) == SYSERR) {
		kprintf("[XBOOT] Xinu load failed\n");
		return SYSERR;
	}

	/* Jump to the loaded Xinu image	*/

	if(xinu_image_valid) {
		
		xbootjmp((void*)boot_hdr.xboot_branch_addr);
		
		/* xbootjmp should not return but if it does, print	*/
		/*	an error message to the user 			*/
		kprintf("[XBOOT] Error: branch to Xinu image failed\n");
	}

	return SYSERR;
}
