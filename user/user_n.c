/*
** Author:	Sol Boucher
**
** Description: N userspace process
*/

#include "userspace.h"

/*
** User N is like user M, except that it runs at high priority.
*/

int main( void ) {
	int i, j;
	prio_t old;

	old = setprio( PRIO_HIGH );
	c_printf( "User N running @ %d, old %d\n", PRIO_HIGH, old );
	writech( 'N' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'N' );
	}

	c_puts( "User N exiting\n" );
	exit();
}
