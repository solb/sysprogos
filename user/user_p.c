/*
** Author:	Sol Boucher
**
** Description: P userspace process
*/

#include "userspace.h"

/*
** User P iterates three times.  Each iteration sleeps for two seconds,
** then gets and prints the system time.
*/

int main( void ) {
	time_t time;
	int i;

	time = gettime();
	c_printf( "User P running, start at %08x\n", time );

	writech( 'P' );

	for( i = 0; i < 3; ++i ) {
		sleep( seconds_to_ms(2) );
		time = gettime();
		c_printf( "User P reporting time %08x\n", time );
		writech( 'P' );
	}

	c_printf( "User P exiting\n" );
	exit();
}
