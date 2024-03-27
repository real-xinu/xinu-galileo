/* device.h - isbaddev */

#define	DEVNAMLEN	16	/* Maximum size of a device name	*/

/* Macro used to verify device ID is valid  */

#define isbaddev(f)  ( ((f) < 0) | ((f) >= NDEVS) )
