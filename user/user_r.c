/*
** Author:	Sol Boucher
**
** Description: R userspace process
*/

#include "userspace.h"

/*
** User R loops 3 times reading/writing, then exits.
*/

int main( void ) {
	int i;
	char ch = '&';

	c_puts( "User R running\n" );
	sleep( seconds_to_ms(10) );
	for( i = 0; i < 3; ++i ) {
		do {
			writech( 'R' );
			ch = readch();
			if( ch == -1 ) {	/* wait a bit */
				sleep( seconds_to_ms(1) );
			}
		} while( ch == -1 );
		writech( ch );
	}

	c_puts( "User R exiting\n" );
	exit();
}
