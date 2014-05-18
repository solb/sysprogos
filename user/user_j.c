/*
** Author:	Sol Boucher
**
** Description: J userspace process
*/

#include "userspace.h"

/*
** User J tries to spawn 2*N_PROCESSES copies of user_y.
*/

int main( void ) {
	int i;
	int pid;

	c_puts( "User J running\n" );
	writech( 'J' );

	for( i = 0; i < N_PROCESSES * 2 ; ++i ) {
		pid = spawn( "/USER_Y.B" );
		if( pid < 0 ) {
			writech( 'j' );
			c_printf( "User J spawn() #%d failed\n", i );
		} else {
			writech( 'J' );
		}
	}

	c_puts( "User J exiting\n" );
	exit();
}
