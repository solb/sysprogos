/*
** Author:	Sol Boucher
**
** Description: L userspace process
*/

#include "userspace.h"

/*
** User L is like user K, except that it prints times and doesn't sleep
** each time, it just preempts itself.
*/

int main( void ) {
	int i;
	int pid;
	time_t time;

	time = gettime();
	c_printf( "User L running, initial time %u\n", time );
	writech( 'L' );

	for( i = 0; i < 3 ; ++i ) {
		time = gettime();
		pid = spawn( "/USER_X.B" );
		if( pid < 0 ) {
			c_printf( "User L spawn() #%d failed @%08u\n", i, time );
		} else {
			sleep( 0 );
			writech( 'L' );
		}
	}

	time = gettime();
	c_printf( "User L exiting at time %u\n", time );
	exit();
}
