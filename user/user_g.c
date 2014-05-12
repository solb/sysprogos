/*
** Author:	Sol Boucher
**
** Description: G userspace process
*/

#include "userspace.h"

/*
** Users E, F, and G test the sleep facility.
**
** User E sleeps for 15 seconds at a time.
*/

int main( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User G (%d) running\n", pid );
	writech( 'G' );
	for( i = 0; i < 5; ++i ) {
		sleep( seconds_to_ms(15) );
		writech( 'G' );
	}

	c_puts( "User G exiting\n" );
	exit();
}
