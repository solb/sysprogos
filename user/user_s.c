/*
** Author:	Sol Boucher
**
** Description: S userspace process
*/

#include "userspace.h"

/*
** User S sleeps for 30 seconds at a time, and loops forever.
*/

int main( void ) {
	c_puts( "User S running\n" );
	writech( 'S' );
	for(;;) {
		sleep( seconds_to_ms(30) );
		writech( 'S' );
	}

	c_puts( "User S exiting!?!?!n" );
	exit();
}
