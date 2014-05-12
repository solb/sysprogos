/*
** Author:	Sol Boucher
**
** Description: T userspace process
*/

#include "userspace.h"

/*
** User T changes its priority back and forth several times
*/

int main( void ) {
	prio_t prio1, prio2;
	int i, j;
	pid_t pid;

	pid = getpid();
	prio1 = getprio();
	c_printf( "User T (%d) running, initial prio %d\n", pid, prio1 );
	
	writech( 'T' );
	for( i = 0; i < 20; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'T' );
	}

	prio2 = setprio( PRIO_HIGH );
	prio1 = getprio();
	c_printf( "User T, set prio #1 old %d new %d\n", prio2, prio1 );
	
	writech( 'T' );
	for( i = 0; i < 20; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'T' );
	}

	prio2 = setprio( PRIO_LOW );
	prio1 = getprio();
	c_printf( "User T, set prio #2 old %d new %d\n", prio2, prio1 );

	writech( 'T' );
	for( i = 0; i < 20; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'T' );
	}

	prio2 = setprio( PRIO_STD );
	prio1 = getprio();
	c_printf( "User T, set prio #3 old %d new %d\n", prio2, prio1 );
	
	writech( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'T' );
	}

	c_puts( "User T exiting\n" );
	exit();
}
