/*
** Author:	Sol Boucher
**
** Description: Bad spawn attempt
*/

#include "userspace.h"

/*
** User NULLPTR attempts to dereference a null pointer.
*/

int main( void ) {
	c_printf("About to attempt a NULL spawn...\n");
	spawn(NULL);
	exit();
}
