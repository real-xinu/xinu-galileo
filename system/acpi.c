/* acpi.c - acpi_scan */

#include <xinu.h>

struct acpi_rsdp *acpi_rsdp_ptr;
struct acpi_rsdt *acpi_rsdt_ptr;
struct acpi_madt *acpi_madt_ptr;

/*------------------------------------------------------------------------
 *  acpi_scan  -  Scan the PCI bus
 *------------------------------------------------------------------------
 */
int32 acpi_scan() {

	uint32	*ptr32;
	uint16	*ptr16;
	uint8 	*ptr8, *madt_end, cksum;
	int32	i, num_rsdt_entries;

	for(ptr16 = (uint16 *)ACPI_SCAN_ADDR_START;
		ptr16 < (uint16 *)ACPI_SCAN_ADDR_END; ptr16++) {

		ptr32 = (uint32 *)ptr16;
		if((*ptr32 == ACPI_RSDP_SIGNATURE1) 
			&& (*(ptr32 + 1) == ACPI_RSDP_SIGNATURE2)) { /* RSDP Signature found */

			acpi_rsdp_ptr = (struct acpi_rsdp *)ptr16;

			/* Compute and verify the checksum */
			cksum = 0;
			ptr8 = (uint8 *)ptr16;
			for(i = 0; i < 20; i++) {
				cksum += *ptr8;
				ptr8++;
			}
			if(cksum != 0) {
				acpi_rsdp_ptr = NULL;
				continue;
			}

			kprintf("acpi_scan: RSDP at %08x\n", ptr16);

			/* Get the RSDT Address */
			acpi_rsdt_ptr = (struct acpi_rsdt *)acpi_rsdp_ptr->rsdt_address;

			/* Verify that the address points to a valid RSDT */
			if(acpi_rsdt_ptr->header.signature != ACPI_RSDT_SIGNATURE) {
				return SYSERR;
			}

			/* Compute and verify the RSDT checksum */
			cksum = 0;
			ptr8 = (uint8 *)acpi_rsdt_ptr;
			for(i = 0; i < acpi_rsdt_ptr->header.length; i++) {
				cksum += *ptr8;
				ptr8++;
			}
			if(cksum != 0) {
				kprintf("acpi_scan: Invalid RSDT Checksum\n");
				return SYSERR;
			}

			kprintf("acpi_scan: RSDT at %08x\n", acpi_rsdt_ptr);

			/* Find out the number of RSDT Entries */
			num_rsdt_entries = acpi_rsdt_ptr->header.length - sizeof(struct acpi_sdt_header);

			/* Look through the entries, and see which of them
			 * points to a MADT
			 */
			for(i = 0; i < num_rsdt_entries; i++) {
				acpi_madt_ptr = (struct acpi_madt *)acpi_rsdt_ptr->entry[i];
				if(acpi_madt_ptr->header.signature == ACPI_MADT_SIGNATURE) {
					break;
				}
			}
			if(i >= num_rsdt_entries) {
				return SYSERR;
			}

			/* Compute and verify the MADT Checksum */
			cksum = 0;
			ptr8 = (uint8 *)acpi_madt_ptr;
			for(i = 0; i < acpi_madt_ptr->header.length; i++) {
				cksum += *ptr8;
				ptr8++;
			}
			if(cksum != 0) {
				return SYSERR;
			}

			kprintf("acpi_scan: MADT at %08x\n", acpi_madt_ptr);

			ptr8 = (uint8 *)(acpi_madt_ptr + 1);
			madt_end = (uint8 *)acpi_madt_ptr + acpi_madt_ptr->header.length;
			while(ptr8 < madt_end) {

				if(*ptr8 == ACPI_MADT_LOCAL_APIC_TYPE) { /* Found Local APIC Entry */
					struct acpi_local_apic *lapic = (struct acpi_local_apic *)ptr8;
					if(lapic->flags & 0x00000001) {
						kprintf("acpi_scan: CPU acpi_proc_id %d, apic_id %d\n",
							lapic->acpi_proc_id, lapic->apic_id);
					}
				}
				else if(*ptr8 == ACPI_MADT_IO_APIC_TYPE) { /* Found I/O APIC Entry */
					struct acpi_io_apic *ioapic = (struct acpi_io_apic *)ptr8;
					kprintf("acpi_scan: I/O APIC: address 0x%08x global_sys_int_base %d\n",
						ioapic->io_apic_address, ioapic->global_sys_int_base);
				}
				ptr8 += *(ptr8 + 1);
			}
			return OK;
		}
	}
	return SYSERR;
}
