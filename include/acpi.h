#define ACPI_RSDP_SIGNATURE1	(('R' << 0) | ('S' << 8) | ('D' << 16) | (' ' << 24))
#define ACPI_RSDP_SIGNATURE2	(('P' << 0) | ('T' << 8) | ('R' << 16) | (' ' << 24))

#define ACPI_SCAN_ADDR_START	(0xE0000)
#define ACPI_SCAN_ADDR_END	(0xFFFFF)

#pragma pack(1)
/* Root System Description Pointer */
struct acpi_rsdp {
	uint32	signature1;		/* RSDP Signature low dword	*/
	uint32	signature2;		/* RSDP Signature high dword	*/
	byte	checksum;		/* Checksum of first 20 bytes	*/
	char	oem_id[6];			/* ASCII String			*/
	byte	revision;		/* Revision			*/
	uint32	rsdt_address;		/* RSDT Address			*/
	uint32	length;			/* Length in bytes		*/
	uint32	xsdt_address_lo;	/* XSDT Address low dword	*/
	uint32	xsdt_address_hi;	/* XSDT Address high dword	*/
	byte	ext_checksum;		/* Check of entire table	*/
	byte	reserved[3];		/* Reserved			*/
};
extern struct acpi_rsdp *acpi_rsdp_ptr;

/* System Description Table Header */
struct acpi_sdt_header {
	uint32	signature;		/* Signature		*/
	uint32	length;			/* Length in bytes	*/
	byte	revision;		/* Revision		*/
	byte	checksum;		/* Checksum of table	*/
	char 	oem_id[6];		/* ASCII String		*/
	char	oem_table_id[8];	/* ASCII String		*/
	uint32	eom_revision;	
	uint32	creator_id;
	uint32	creator_revision;
};

#define ACPI_RSDT_SIGNATURE	(('R' << 0) | ('S' << 8) | ('D' << 16) | ('T' << 24))
/* Root System Description Table */
struct acpi_rsdt {
	struct	acpi_sdt_header header;	/* SDT Header	*/
	uint32	entry[];		/* RSDT Entries	*/
};
extern struct acpi_rsdt *acpi_rsdt_ptr;

#define ACPI_MADT_SIGNATURE	(('A' << 0) | ('P' << 8) | ('I' << 16) | ('C' << 24))
/* Multiple APIC Description Table */
struct acpi_madt {
	struct 	acpi_sdt_header	header;	/* SDT HEader				*/
	uint32	local_int_cont_address;	/* Local Interrupt Controller Address	*/
	uint32 flags;			/* Flags				*/
};
extern struct acpi_madt *acpi_madt_ptr;

#define ACPI_MADT_LOCAL_APIC_TYPE	0
#define ACPI_MADT_IO_APIC_TYPE		1

/* Processor Local APIC	*/
struct acpi_local_apic {
	byte	type;		/* Type of entry = 0	*/
	byte	length;		/* Length of entry = 8	*/
	byte	acpi_proc_id;	/* ACPI Processor ID	*/
	byte	apic_id;	/* APIC ID		*/
	uint32	flags;		/* Flags		*/
};

/* I/O APIC */
struct acpi_io_apic {
	byte	type;			/* Type of entry = 1		*/
	byte	length;			/* Length of entry = 12		*/
	byte	io_apic_id;		/* I/O APIC ID			*/
	byte	reserved;		/* Reserved = 0			*/
	uint32	io_apic_address;	/* I/O APIC Address		*/
	uint32	global_sys_int_base;	/* Global System Interrupt Base	*/
};
#pragma pack()
