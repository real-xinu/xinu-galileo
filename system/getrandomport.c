/* getramdomport.c - getrandomport */

#include <xinu.h>


/*------------------------------------------------------------------------
 * getport  -  Retrieve a random port number 
 *------------------------------------------------------------------------
 */
extern netportseed;

uint16 getrandomport(void) {    
    netportseed = 6364136223846793005ULL * netportseed + 1;
    return 50000 + ((uint16) ((netportseed >> 48)) % 15535);
}


