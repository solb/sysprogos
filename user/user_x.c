/*
** Author:	Sol Boucher
**
** Description: X userspace process
*/

#include "userspace.h"

/*
** User X prints X characters 20 times.  It is spawned multiple
** times, and prints its PID and PPID when started and exiting.
*/

int main( void ) {
	int i, j;
	pid_t pid, ppid;

	pid = getpid();
	ppid = getppid();
	c_printf( "User X running, PID %d PPID %d\n", pid, ppid );

	for( i = 0; i < 20 ; ++i ) {
		writech( 'X' );
		for( j = 0; j < DELAY_STD; ++j )
			continue;
	}

	c_printf( "User X exiting, PID %d PPID %d\n", pid, ppid );
	exit();
}
