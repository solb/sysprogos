/*
** Author:	Sol Boucher
**
** Description: B userspace process
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

	c_puts( "User B running\n" );
	ch = writech( 'B' );
	if( ch != 'B' ) {
		c_printf( "User B, write 1 char '%c'\n", ch );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		ch = writech( 'B' );
		if( ch != 'B' ) {
			c_printf( "User B, write 2 char '%c'\n", ch );
		}
	}

	c_puts( "User B exiting\n" );
	exit();

	ch = writech( 'b' );	/* shouldn't happen! */
	if( ch != 'b' ) {
		c_printf( "User B, write 3 char '%c'\n", ch );
	}
}
