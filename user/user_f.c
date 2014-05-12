/*
** Author:	Sol Boucher
**
** Description: F userspace process
*/

#include "userspace.h"

/*
** Users E, F, and G test the sleep facility.
**
** User F sleeps for 5 seconds at a time.
*/

int main( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User F (%d) running\n", pid );
	writech( 'F' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( seconds_to_ms(5) );
		writech( 'F' );
	}

	c_puts( "User F exiting\n" );
	exit();
}
