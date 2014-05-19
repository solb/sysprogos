/*
** Author:	Sol Boucher
**
** Description: Userspace init/idle process
*/

#include "userspace.h"

/*
** Initial process; it starts the other top-level user processes.
** It then becomes the system idle process!
*/
int main( void ) {
	int i;
	pid_t pid;
	prio_t prio, prio2;
	time_t time;

	pid = getpid();
	if(pid != 1) {
		c_printf( "init refusing to run with pid %d\n", pid );
		return;
	}

	c_puts( "Starting init program\n" );

	prio = setprio( PRIO_HIGH );
	prio = getprio();
	c_printf( "Init started, now at priority %d\n", prio );
	for(unsigned waste = 0; waste < 1000000000; ++waste);

	//clear the serial console
	writech('');

	pid = spawn( "/shell.b" );
	if( pid < 0 ) {
		c_printf( "init, spawn() shell failed\n" );
		exit();
	}

	/*
	** At this point, we go into an infinite loop at low
	** priority, (not) printing dot characters.
	*/

	time = gettime();
	prio = setprio( PRIO_LOW );
	prio2 = getprio();
	c_printf( "Init -> Idle @ %08x, old prio %d, now %d\n",
		time, prio, prio2 );

	//writech( '.' );

	for(;;) {
		for( i = 0; i < DELAY_LONG; ++i )
			continue;
		//writech( '.' );
	}

	/*
	** SHOULD NEVER REACH HERE
	*/

	time = gettime();
	c_printf( "+++ INIT/IDLE EXITING @ %u!?!?!\n", time );
}
