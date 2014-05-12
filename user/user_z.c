/*
** Author:	Sol Boucher
**
** Description: Z userspace process
*/

#include "userspace.h"

/*
** User Z prints Z characters 10 times.
*/

int main( void ) {
	int i, j;
	pid_t pid, ppid;

	pid = getpid();
	ppid = getppid();

	c_printf( "User Z (%d) running, parent %d\n", pid, ppid );
	for( i = 0; i < 10 ; ++i ) {
		writech( 'Z' );
		for( j = 0; j < DELAY_STD; ++j )
			continue;
	}

	c_printf( "User Z (%d) exiting, parent %d\n", pid, ppid );
	exit();
}
