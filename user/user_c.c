/*
** Author:	Sol Boucher
**
** Description: C userspace process
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

	c_puts( "User C running\n" );
	ch = writech( 'C' );
	if( ch != 'C' ) {
		c_printf( "User C, write 1 char '%c'\n", ch );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		ch = writech( 'C' );
		if( ch != 'C' ) {
			c_printf( "User C, write 2 char '%c'\n", ch );
		}
	}

	c_puts( "User C exiting\n" );
	exit();

	ch = writech( 'c' );	/* shouldn't happen! */
	if( ch != 'c' ) {
		c_printf( "User C, write 3 char '%c'\n", ch );
	}
}
