/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <xinu.h>


extern	devcall	ioerr(void);
extern	devcall	ionull(void);

/* Device independent I/O switch */

struct	dentry	devtab[NDEVS] =
{
/**
 * Format of entries is:
 * dev-number, minor-number, dev-name,
 * init, open, close,
 * read, write, seek,
 * getc, putc, control,
 * dev-csr-address, intr-handler, irq
 */

/* CONSOLE is tty */
	{ 0, 0, "CONSOLE",
	  (void *)ttyInit, (void *)ionull, (void *)ionull,
	  (void *)ttyRead, (void *)ttyWrite, (void *)ioerr,
	  (void *)ttyGetc, (void *)ttyPutc, (void *)ttyControl,
	  (void *)0x3f8, (void *)ttyDispatch, 42 },

/* NULLDEV is null */
	{ 1, 0, "NULLDEV",
	  (void *)ionull, (void *)ionull, (void *)ionull,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)ionull, (void *)ionull, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 },

/* ETHER0 is eth */
	{ 2, 0, "ETHER0",
	  (void *)ethInit, (void *)ioerr, (void *)ioerr,
	  (void *)ethRead, (void *)ethWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ethControl,
	  (void *)0x0, (void *)ethDispatch, 43 },

/* NAMESPACE is nam */
	{ 3, 0, "NAMESPACE",
	  (void *)namInit, (void *)namOpen, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ioerr, 0 },

/* RDISK is rds */
	{ 4, 0, "RDISK",
	  (void *)rdsInit, (void *)rdsOpen, (void *)rdsClose,
	  (void *)rdsRead, (void *)rdsWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)rdsControl,
	  (void *)0x0, (void *)ionull, 0 },

/* RAM0 is ram */
	{ 5, 0, "RAM0",
	  (void *)ramInit, (void *)ramOpen, (void *)ramClose,
	  (void *)ramRead, (void *)ramWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)ionull, 0 },

/* SDMC is sdmc */
	{ 6, 0, "SDMC",
	  (void *)sdmcInit, (void *)sdmcOpen, (void *)sdmcClose,
	  (void *)sdmcRead, (void *)sdmcWrite, (void *)ioerr,
	  (void *)ioerr, (void *)ioerr, (void *)ioerr,
	  (void *)0x0, (void *)sdmcDispatch, 41 }
};
