/*
** Author:	Sol Boucher
**
** Description: W userspace process
*/

#include "userspace.h"

/*
** User W prints W characters 20 times, sleeping 3 seconds between.
*/

int main( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User W running, PID %d\n", pid );
	for( i = 0; i < 20 ; ++i ) {
		writech( 'W' );
		sleep( seconds_to_ms(3) );
	}

	c_printf( "User W exiting, PID %d\n", pid );
	exit();
}
