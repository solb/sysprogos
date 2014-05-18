/*
** Author:	Sol Boucher
**
** Description: H userspace process
*/

#include "userspace.h"

/*
** User H is like A-C except it only loops 5 times and doesn't
** call exit().
*/

int main( void ) {
	int i, j;

	c_puts( "User H running\n" );
	writech( 'H' );
	for( i = 0; i < 5; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'H' );
	}

	c_puts( "User H returning without exiting!\n" );
}
