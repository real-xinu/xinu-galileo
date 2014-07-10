/* xsh_led.c - xsh_led */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

/*------------------------------------------------------------------------
 * xsh_led - shell command to turn LEDs on or off
 *------------------------------------------------------------------------
 */
shellcmd xsh_led(int nargs, char *args[])
{
	uint32 led;                  /* specific LED to control */

	/* For argument '--help', emit help about the 'led' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s LED_name state\n\n", args[0]);
		printf("Description:\n");
		printf("\tTurns a front-panel LED on or off\n");
		printf("Options:\n");
		printf("\tLED_name:\tdmz, wlan, power, ciscow, ciscoo\n");
		printf("\tState:\t\ton, off\n");

		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid number of arguments */

	if (nargs != 3) {
		fprintf(stderr, "%s: error in arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	led = 0;

	/* cfind led to change */

	if (strncmp(args[1], "dmz", 3) == 0) {
		led = GPIO_LED_DMZ;
	} else if (strncmp(args[1], "wlan", 4) == 0) {
		led = GPIO_LED_WLAN;
	} else if (strncmp(args[1], "power", 5) == 0) {
		led = GPIO1;
	} else if (strncmp(args[1], "ciscow", 6) == 0) {
		led = GPIO_LED_CISCOWHT;
	} else if (strncmp(args[1], "ciscoo", 6) == 0) {
		led = GPIO_LED_CISCOONG;
	} else {
		fprintf(stderr, "%s: LED name %s is invalid\n",
			args[0], args[1]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	/* Change led state */

	if (strncmp(args[2], "on", 2) == 0) {
		gpioLEDOn(led);
	} else if (strncmp(args[2], "off", 3) == 0) {
		gpioLEDOff(led);
	} else {
		fprintf(stderr, "%s: invalid state\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
	}
	return 0;
}
