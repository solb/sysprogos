/*
** Author:	Sol Boucher
**
** Description: Bad synchronous spawn attempt
*/

#include "userspace.h"

/*
** Attempts to spawn from a null pointer.
*/

int main( void ) {
	c_printf("About to attempt a NULL spawn...\n");
	syncspawn(NULL);
	exit();
}
