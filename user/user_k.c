/*
** Author:	Sol Boucher
**
** Description: K userspace process
*/

#include "userspace.h"

/*
** User K prints, goes into a loop which runs three times, and exits.
** In the loop, it does a spawn of user_x, sleeps 30 seconds, and prints.
*/

int main( void ) {
	int i;
	int pid;

	c_puts( "User K running\n" );
	writech( 'K' );

	for( i = 0; i < 3 ; ++i ) {
		pid = spawn( "\\USER_X  B  " );
		if( pid < 0 ) {
			c_printf( "User K spawn() #%d failed\n", i );
		} else {
			sleep( seconds_to_ms(30) );
			writech( 'K' );
		}
	}

	c_puts( "User K exiting\n" );
	exit();
}
