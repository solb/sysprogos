/*
** Author:	Sol Boucher
**
** Description: Userspace init program
*/

#include "init.h"

/*
** Initial process; it starts the other top-level user processes.
*/
int main( void ) {
	int i;
	pid_t pid;
	prio_t prio, prio2;
	time_t time;

	c_puts( "Starting init program\n" );

	prio = setprio( PRIO_HIGH );
	prio = getprio();
	c_printf( "Init started, now at priority %d\n", prio );
	for(unsigned waste = 0; waste < 1000000000; ++waste);

	writech( '$' );

#ifdef SPAWN_A
	pid = spawn( "/USER_A  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user A failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_B
	pid = spawn( "/USER_B  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user B failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_C
	pid = spawn( "/USER_C  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user C failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_D
	pid = spawn( "/USER_D  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user D failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_E
	pid = spawn( "/USER_E  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user E failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_F
	pid = spawn( "/USER_F  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user F failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_G
	pid = spawn( "/USER_G  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user G failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_H
	pid = spawn( "/USER_H  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user H failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_J
	pid = spawn( "/USER_J  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user J failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_K
	pid = spawn( "/USER_K  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user K failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_L
	pid = spawn( "/USER_L  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user L failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_M
	pid = spawn( "/USER_M  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user M failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_N
	pid = spawn( "/USER_N  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user N failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_P
	pid = spawn( "/USER_P  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user P failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_Q
	pid = spawn( "/USER_Q  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user Q failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_R
	pid = spawn( "/USER_R  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user R failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_S
	pid = spawn( "/USER_S  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user S failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_T
	pid = spawn( "/USER_T  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user T failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_U
	pid = spawn( "/USER_U  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user U failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_V
	pid = spawn( "/USER_V  B  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user V failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_NULLPTR
	pid = spawn( "/DREF_NULB  " );
	if( pid < 0 ) {
		c_printf( "init, spawn() user NULLPTR failed\n" );
		exit();
	}
#endif

	writech( '!' );

	/*
	** At this point, we go into an infinite loop at low
	** priority, printing dot characters.
	*/

	time = gettime();
	prio = setprio( PRIO_LOW );
	prio2 = getprio();
	//c_printf( "Init -> Idle @ %08x, old prio %d, now %d\n",
		//time, prio, prio2 );

	writech( '.' );

	for(;;) {
		for( i = 0; i < DELAY_LONG; ++i )
			continue;
		writech( '.' );
	}

	/*
	** SHOULD NEVER REACH HERE
	*/

	time = gettime();
	c_printf( "+++ INIT/IDLE EXITING @ %u!?!?!\n", time );

	exit();
}
