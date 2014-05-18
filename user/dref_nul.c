/*
** Author:	Sol Boucher
**
** Description: Protection-checking user process
*/

#include "userspace.h"

/*
** User NULLPTR attempts to dereference a null pointer.
*/

int main( void ) {
	c_printf( "About to dereference a NULL pointer...\n" );
	uint64_t *addr = NULL;
	uint64_t value = *addr;
	c_printf( "ERROR: Successfully dereferenced NULL and found %d!\n", value );
	exit();
}
