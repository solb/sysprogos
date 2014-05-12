/*
** Author:	Sol Boucher
**
** Description: D userspace process
*/

#include "userspace.h"

/*
** User D spawns user Z.
*/

int main( void ) {
	pid_t pid;

	c_puts( "User D running\n" );
	writech( 'D' );

	pid = spawn( "\\USER_Z  B  " );
	if( pid < 0 ) {
		c_printf( "User D spawn() failed\n" );
	}

	writech( 'D' );

	c_puts( "User D exiting\n" );
	exit();
}
