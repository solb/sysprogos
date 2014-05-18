/*
** Author:	Sol Boucher
**
** Description: E userspace process
*/

#include "userspace.h"

/*
** Users E, F, and G test the sleep facility.
**
** User E sleeps for 10 seconds at a time.
*/

int main( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User E (%d) running\n", pid );
	writech( 'E' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( seconds_to_ms(10) );
		writech( 'E' );
	}

	c_puts( "User E exiting\n" );
	exit();
}
