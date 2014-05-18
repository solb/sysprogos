/*
** Author:	Sol Boucher
**
** Description: M userspace process
*/

#include "userspace.h"

/*
** User M is like A except that it runs at low priority.
*/

int main( void ) {
	int i, j;
	prio_t old;

	old = setprio( PRIO_LOW );
	c_printf( "User M running @ %d, old %d\n", PRIO_LOW, old );
	writech( 'M' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'M' );
	}

	c_puts( "User M exiting\n" );
	exit();
}
