/*
** Author:	Sol Boucher
**
** Description: Y userspace process
*/

#include "userspace.h"

/*
** User Y prints Y characters 10 times.
*/

int main( void ) {
	int i, j;

	c_puts( "User Y running\n" );
	for( i = 0; i < 10 ; ++i ) {
		writech( 'Y' );
		for( j = 0; j < DELAY_ALT; ++j )
			continue;
		sleep( seconds_to_ms(1) );
	}

	c_puts( "User Y exiting\n" );
	exit();
}
