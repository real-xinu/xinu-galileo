#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define XBOOT_HEADER_VERSION  0
#define XBOOT_HEADER_SIG      "Xinu"
#define XINU_LOAD_ADDRESS     0x100000

struct xboot_hdr {
	char xboot_sig1[4];		/* xboot signature - Xinu 	*/
	uint32_t xboot_hdr_ver;		/* xboot header version - 0 	*/
	uint32_t xboot_load_addr;		/* Address to load Xinu 	*/
	uint32_t xboot_branch_addr; 	/* Address to branch to after 	*/
					/*	loading Xinu 		*/
	uint32_t xboot_file_size;		/* Xinu image size (does not 	*/
					/*	include xboot header)	*/
	uint32_t xinu_crc32;		/* CRC32 checksum of the Xinu 	*/
					/*	image 			*/
	char xboot_sig2[4];		/* xboot signature - Xinu 	*/
	char xboot_pad[484];		/* xboot header pad - should be	*/
					/*	all zeros 		*/
};

/* Values for computing CRC32 used to checksum the Xinu image */
uint32_t crc_table[] = {
	0x4DBDF21C, 0x500AE278, 0x76D3D2D4, 0x6B64C2B0,
	0x3B61B38C, 0x26D6A3E8, 0x000F9344, 0x1DB88320,
	0xA005713C, 0xBDB26158, 0x9B6B51F4, 0x86DC4190,
	0xD6D930AC, 0xCB6E20C8, 0xEDB71064, 0xF0000000
};

/*------------------------------------------------------------------------
 * crc32 - Computes the CRC32 checksum of the given data
 *------------------------------------------------------------------------
 */
uint32_t crc32(
	 uint8_t* data, 		/* Data to checksum		*/
	 uint32_t data_size		/* Size of data			*/
	)
{
	uint32_t i, crc = 0;

	for (i = 0; i < data_size; i++) {
		crc = (crc >> 4) ^ crc_table[(crc ^
			(data[i] >> 0)) & 0x0F];  /* lower nibble */
		crc = (crc >> 4) ^ crc_table[(crc ^
			(data[i] >> 4)) & 0x0F];  /* upper nibble */
	}
	
	return crc;
}

int main(int argc, char* argv[])
{
    int xinu_fd;
    uint8_t* xinu_buffer;
    struct xboot_hdr xbin_hdr;

    if (argc != 3) {
        fprintf(stderr, "Usage: buildxbin xinu.bin xinu.xbin\n");
        return 1;
    }

    /* Initialize the XBIN header */
    memset(&xbin_hdr, 0, sizeof(struct xboot_hdr));
    xbin_hdr.xboot_hdr_ver = XBOOT_HEADER_VERSION;
    xbin_hdr.xboot_load_addr = XINU_LOAD_ADDRESS;
    xbin_hdr.xboot_branch_addr = XINU_LOAD_ADDRESS;
    memcpy(&(xbin_hdr.xboot_sig1), XBOOT_HEADER_SIG, sizeof(xbin_hdr.xboot_sig1));
    memcpy(&(xbin_hdr.xboot_sig2), XBOOT_HEADER_SIG, sizeof(xbin_hdr.xboot_sig2));

    /* Read the XINU binary image */
    xinu_fd = open(argv[1], O_RDONLY);
    if(xinu_fd == -1) {
        fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    xbin_hdr.xboot_file_size = lseek(xinu_fd, 0, SEEK_END);
    lseek(xinu_fd, 0, SEEK_SET);
    xinu_buffer = (uint8_t*)malloc(xbin_hdr.xboot_file_size);
    if(read(xinu_fd, xinu_buffer, xbin_hdr.xboot_file_size) == -1) {
        fprintf(stderr, "Unale to read: %s: %s\n", argv[1], strerror(errno));
        free(xinu_buffer);
        close(xinu_fd);
        return 1;
    }
    close(xinu_fd);

    /* Compute the CRC32 */
    xbin_hdr.xinu_crc32 = crc32(xinu_buffer, xbin_hdr.xboot_file_size);

    /* Write the XINU xbin */
    xinu_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if(xinu_fd == -1) {
        fprintf(stderr, "Unable to open %s: %s\n", argv[2], strerror(errno));
        free(xinu_buffer);
        return 1;
    }

    if(write(xinu_fd, &(xbin_hdr), sizeof(struct xboot_hdr)) == -1) {
        fprintf(stderr, "Unable to write %s: %s\n", argv[2], strerror(errno));
        free(xinu_buffer);
        close(xinu_fd);
        return 1;
    }
    if(write(xinu_fd, xinu_buffer, xbin_hdr.xboot_file_size) == -1) {
        fprintf(stderr, "Unable to write %s: %s\n", argv[2], strerror(errno));
        free(xinu_buffer);
        close(xinu_fd);
        return 1;
    }
    free(xinu_buffer);
    close(xinu_fd);

    return 0;
}
