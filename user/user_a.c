/*
** Author:	Sol Boucher
**
** Description: A userspace process
*/

#include "userspace.h"

/*
** Users A, B, and C are identical, except for the character they
** print out via writech().  Each prints its ID, then loops 30
** times delaying and printing, before exiting.  They also verify
** the status return from writech().
*/

int main( void ) {
	int i, j, ch;

	c_puts( "User A running\n" );
	ch = writech( 'A' );
	if( ch != 'A' ) {
		c_printf( "User A, write 1 char '%c'\n", ch );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		ch = writech( 'A' );
		if( ch != 'A' ) {
			c_printf( "User A, write 2 char '%c'\n", ch );
		}
	}

	c_puts( "User A exiting\n" );
	exit();

	ch = writech( 'a' );	/* shouldn't happen! */
	if( ch != 'a' ) {
		c_printf( "User A, write 3 char '%c'\n", ch );
	}
}
