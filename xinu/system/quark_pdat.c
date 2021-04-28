/* quark_pdat.c - Functions for retrieving platform data from the quark SPI flash */

#include <xinu.h>

/*------------------------------------------------------------------------
 * get_quark_pdat_entry_data_by_id - Retrieve platform data by type identifier
 *------------------------------------------------------------------------
 */
int get_quark_pdat_entry_data_by_id(uint16 type_id, char* buf, uint32 size)
{
	uint32 offset = 0;
	struct quark_pdat_header* header = (struct quark_pdat_header*)QUARK_PDAT_MEMORY_LOC;
	
	if(memcmp(header->quark_pdat_magic, QUARK_PDAT_MAGIC, 4) != 0) {
		kprintf("Invalid platform data magic\n");
		return SYSERR;
	}
	
	struct quark_pdat_entry* entry = (struct quark_pdat_entry*)(header + 1);
	while(offset < header->quark_pdat_len) {
	
		/* Found the requested ID, copy the data to the caller's buffer */
		if(entry->pdat_entry_id == type_id) {
			memset(buf, 0x00, size);
			size = size < entry->pdat_entry_len ? size : entry->pdat_entry_len;
			memcpy(buf, (char*)(entry + 1), size);
			return size;
		}
		
		entry = (struct quark_pdat_entry*)(((char*)entry) + (sizeof(struct quark_pdat_entry) + entry->pdat_entry_len));
		offset += (sizeof(struct quark_pdat_entry) + entry->pdat_entry_len);
	}
	
	return SYSERR;
}

/*------------------------------------------------------------------------
 * get_quark_pdat_entry_data_by_desc - Retrieve platform data by description
 *------------------------------------------------------------------------
 */
int get_quark_pdat_entry_data_by_desc(const char* entry_desc, char* buf, uint32 size)
{
	uint32 offset = 0;
	struct quark_pdat_header* header = (struct quark_pdat_header*)QUARK_PDAT_MEMORY_LOC;
	
	if(memcmp(header->quark_pdat_magic, QUARK_PDAT_MAGIC, 4) != 0) {
		kprintf("Invalid platform data magic\n");
		return SYSERR;
	}
	
	struct quark_pdat_entry* entry = (struct quark_pdat_entry*)(header + 1);
	while(offset < header->quark_pdat_len) {
	
		/* Found the requested description, copy the data to the caller's buffer */
		if(strncmp(entry->pdat_entry_desc, entry_desc, 10) == 0) {
			memset(buf, 0x00, size);
			size = size < entry->pdat_entry_len ? size : entry->pdat_entry_len;
			memcpy(buf, (char*)(entry + 1), size);
			return size;
		}
		
		entry = (struct quark_pdat_entry*)(((char*)entry) + (sizeof(struct quark_pdat_entry) + entry->pdat_entry_len));
		offset += (sizeof(struct quark_pdat_entry) + entry->pdat_entry_len);
	}
	
	return SYSERR;
}