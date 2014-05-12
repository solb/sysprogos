/*
** Author:	Sol Boucher
**
** Description: Q userspace process
*/

#include "userspace.h"

/*
** User Q does a bogus system call
*/

int main( void ) {
	c_puts( "User Q running\n" );
	writech( 'Q' );
	bogus();
	c_puts( "User Q returned from bogus syscall!?!?!\n" );
	exit();
}
