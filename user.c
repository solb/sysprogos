/*
** SCCS ID:	@(#)user.c	1.1	4/9/14
**
** File:	user.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	User routines.
*/

#include "common.h"

#include "user.h"

#include "c_io.h"

/*
** USER PROCESSES
**
** Each is designed to test some facility of the OS; see the user.h
** header file for a description of which system calls are used by
** each user process.
**
** Output from user processes is always alphabetic.  Uppercase 
** characters are "expected" output; lowercase are "erroneous"
** output.
**
** More specific information about each user process can be found in
** the header comment for that function (below).
**
** To spawn a specific user process, uncomment its SPAWN_x
** definition in the user.h header file.
*/

/*
** Prototypes for all one-letter user main routines (even
** ones that may not exist, for completeness)
*/

void user_a( void ); void user_b( void ); void user_c( void );
void user_d( void ); void user_e( void ); void user_f( void );
void user_g( void ); void user_h( void ); void user_i( void );
void user_j( void ); void user_k( void ); void user_l( void );
void user_m( void ); void user_n( void ); void user_o( void );
void user_p( void ); void user_q( void ); void user_r( void );
void user_s( void ); void user_t( void ); void user_u( void );
void user_v( void ); void user_w( void ); void user_x( void );
void user_y( void ); void user_z( void );

/*
** Users A, B, and C are identical, except for the character they
** print out via writech().  Each prints its ID, then loops 30
** times delaying and printing, before exiting.  They also verify
** the status return from writech().
*/

void user_a( void ) {
	int i, j, ch;

	c_puts( "User A running\n" );
	ch = writech( 'A' );
	if( ch != 'A' ) {
		c_printf( "User A, write 1 char '%c'\n", ch );
	}
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		ch = writech( 'A' );
		if( ch != 'A' ) {
			c_printf( "User A, write 2 char '%c'\n", ch );
		}
	}

	c_puts( "User A exiting\n" );
	exit();

	ch = writech( 'a' );	/* shouldn't happen! */
	if( ch != 'a' ) {
		c_printf( "User A, write 3 char '%c'\n", ch );
	}
}

void user_b( void ) {
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

void user_c( void ) {
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

/*
** User D spawns user Z.
*/

void user_d( void ) {
	pid_t pid;

	c_puts( "User D running\n" );
	writech( 'D' );

	pid = spawn( user_z );
	if( pid < 0 ) {
		c_printf( "User D spawn() failed\n" );
	}

	writech( 'D' );

	c_puts( "User D exiting\n" );
	exit();
}


/*
** Users E, F, and G test the sleep facility.
**
** User E sleeps for 10 seconds at a time.
*/

void user_e( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User E (%d) running\n", pid );
	writech( 'E' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( seconds_to_ms(10) );
		writech( 'E' );
	}

	c_puts( "User E exiting\n" );
	exit();
}


/*
** User F sleeps for 5 seconds at a time.
*/

void user_f( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User F (%d) running\n", pid );
	writech( 'F' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( seconds_to_ms(5) );
		writech( 'F' );
	}

	c_puts( "User F exiting\n" );
	exit();
}


/*
** User G sleeps for 15 seconds at a time.
*/

void user_g( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User G (%d) running\n", pid );
	writech( 'G' );
	for( i = 0; i < 5; ++i ) {
		sleep( seconds_to_ms(15) );
		writech( 'G' );
	}

	c_puts( "User G exiting\n" );
	exit();
}


/*
** User H is like A-C except it only loops 5 times and doesn't
** call exit().
*/

void user_h( void ) {
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


/*
** User J tries to spawn 2*N_PROCESSES copies of user_y.
*/

void user_j( void ) {
	int i;
	int pid;

	c_puts( "User J running\n" );
	writech( 'J' );

	for( i = 0; i < N_PROCESSES * 2 ; ++i ) {
		pid = spawn( user_y );
		if( pid < 0 ) {
			writech( 'j' );
			c_printf( "User J spawn() #%d failed\n", i );
		} else {
			writech( 'J' );
		}
	}

	c_puts( "User J exiting\n" );
	exit();
}


/*
** User K prints, goes into a loop which runs three times, and exits.
** In the loop, it does a spawn of user_x, sleeps 30 seconds, and prints.
*/

void user_k( void ) {
	int i;
	int pid;

	c_puts( "User K running\n" );
	writech( 'K' );

	for( i = 0; i < 3 ; ++i ) {
		pid = spawn( user_x );
		if( pid < 0 ) {
			c_printf( "User K spawn() #%d failed\n", i );
		} else {
			sleep( seconds_to_ms(30) );
			writech( 'K' );
		}
	}

	c_puts( "User K exiting\n" );
	exit();
}


/*
** User L is like user K, except that it prints times and doesn't sleep
** each time, it just preempts itself.
*/

void user_l( void ) {
	int i;
	int pid;
	time_t time;

	time = gettime();
	c_printf( "User L running, initial time %u\n", time );
	writech( 'L' );

	for( i = 0; i < 3 ; ++i ) {
		time = gettime();
		pid = spawn( user_x );
		if( pid < 0 ) {
			c_printf( "User L spawn() #%d failed @%08u\n", i, time );
		} else {
			sleep( 0 );
			writech( 'L' );
		}
	}

	time = gettime();
	c_printf( "User L exiting at time %u\n", time );
	exit();
}


/*
** User M is like A except that it runs at low priority.
*/

void user_m( void ) {
	int i, j;
	prio_t old;

	old = setprio( PRIO_LOW );
	c_printf( "User M running @ %d, old %d\n", PRIO_LOW, old );
	writech( 'M' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'M' );
	}

	c_puts( "User M exiting\n" );
	exit();
}


/*
** User N is like user M, except that it runs at high priority.
*/

void user_n( void ) {
	int i, j;
	prio_t old;

	old = setprio( PRIO_HIGH );
	c_printf( "User N running @ %d, old %d\n", PRIO_HIGH, old );
	writech( 'N' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writech( 'N' );
	}

	c_puts( "User N exiting\n" );
	exit();
}


/*
** User P iterates three times.  Each iteration sleeps for two seconds,
** then gets and prints the system time.
*/

void user_p( void ) {
	time_t time;
	int i;

	time = gettime();
	c_printf( "User P running, start at %08x\n", time );

	writech( 'P' );

	for( i = 0; i < 3; ++i ) {
		sleep( seconds_to_ms(2) );
		time = gettime();
		c_printf( "User P reporting time %08x\n", time );
		writech( 'P' );
	}

	c_printf( "User P exiting\n" );
	exit();
}


/*
** User Q does a bogus system call
*/

void user_q( void ) {

	c_puts( "User Q running\n" );
	writech( 'Q' );
	bogus();
	c_puts( "User Q returned from bogus syscall!?!?!\n" );
	exit();

}


/*
** User R loops 3 times reading/writing, then exits.
*/

void user_r( void ) {
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


/*
** User S sleeps for 30 seconds at a time, and loops forever.
*/

void user_s( void ) {

	c_puts( "User S running\n" );
	writech( 'S' );
	for(;;) {
		sleep( seconds_to_ms(30) );
		writech( 'S' );
	}

	c_puts( "User S exiting!?!?!n" );
	exit();

}


/*
** User T changes its priority back and forth several times
*/

void user_t( void ) {
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


/*
** User U prints strings.
*/

void user_u( void ) {
	int i;

	c_printf( "User U running\n" );
	i = writes( "Us1U" );
	c_printf( "User U string 1 %d chars\n", i );
	i = writes( "UHello, world!U" );
	c_printf( "User U string 2 %d chars\n", i );
	i = writes( "ULet's try a really long string for a changeU" );
	c_printf( "User U string 3 %d chars\n", i );

	c_printf( "User U exiting\n" );
	exit();
}


/*
** User W prints W characters 20 times, sleeping 3 seconds between.
*/

void user_w( void ) {
	int i;
	pid_t pid;

	pid = getpid();
	c_printf( "User W running, PID %d\n", pid );
	for( i = 0; i < 20 ; ++i ) {
		writech( 'W' );
		sleep( seconds_to_ms(3) );
	}

	c_printf( "User W exiting, PID %d\n", pid );
	exit();

}


/*
** User X prints X characters 20 times.  It is spawned multiple
** times, and prints its PID and PPID when started and exiting.
*/

void user_x( void ) {
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


/*
** User Y prints Y characters 10 times.
*/

void user_y( void ) {
	int i, j;

	c_puts( "User Y running\n" );
	for( i = 0; i < 10 ; ++i ) {
		writech( 'Y' );
		for( j = 0; j < DELAY_ALT; ++j )
			continue;
		sleep( seconds_to_ms(1) );
	}

	c_puts( "User Y exiting\n" );
	exit();

}


/*
** User Z prints Z characters 10 times.
*/

void user_z( void ) {
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

/*
 * User NULLPTR attempts to dereference a null pointer.
 */
void user_nullptr( void ) {
	c_printf( "About to dereference a NULL pointer...\n" );
	uint64_t *addr = NULL;
	uint64_t value = *addr;
	c_printf( "ERROR: Successfully dereferenced NULL and found %d!\n", value );
	exit();
}

/*
** User BELOWSTACKS attempts to dereference the memory just below the userspace stacks.
*/
void user_belowstacks( void ) {
	uint64_t *addr = (uint64_t *)0x60000 - 0x19000;
	uint64_t value = *addr;
	c_printf( "Value at beginning of user stack space: %d\n", value );
	c_printf( "About to dereference below user stacks...\n" );
	--addr;
	value = *addr;
	c_printf( "ERROR: Allowed to steal this value from below the user stacks: %d\n", value );
	exit();
}

/*
** User ABOVEUSER attempts to dereference the memory just above the userspace bss.
*/
void user_aboveuser( void ) {
	uint64_t *addr = (uint64_t *)0x70000;
	--addr;
	uint64_t value = *addr;
	c_printf( "Value at end of userland-accessible region: %d\n", value );
	c_printf( "About to dereference past the end of userland...\n" );
	++addr;
	value = *addr;
	c_printf( "ERROR: Allowed to steal this value from at world's end: %d\n", value );
	exit();
}

/*
** SYSTEM PROCESSES
*/



/*
** Initial process; it starts the other top-level user processes.
*/

void init( void ) {
	int i;
	pid_t pid;
	prio_t prio, prio2;
	time_t time;

	c_puts( "Starting init()\n" );

	prio = setprio( PRIO_HIGH );
	prio = getprio();
	c_printf( "Init started, now at priority %d\n", prio );
	for(unsigned waste = 0; waste < 1000000000; ++waste);

	writech( '$' );

#ifdef SPAWN_A
	pid = spawn( user_a );
	if( pid < 0 ) {
		c_printf( "init, spawn() user A failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_B
	pid = spawn( user_b );
	if( pid < 0 ) {
		c_printf( "init, spawn() user B failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_C
	pid = spawn( user_c );
	if( pid < 0 ) {
		c_printf( "init, spawn() user C failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_D
	pid = spawn( user_d );
	if( pid < 0 ) {
		c_printf( "init, spawn() user D failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_E
	pid = spawn( user_e );
	if( pid < 0 ) {
		c_printf( "init, spawn() user E failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_F
	pid = spawn( user_f );
	if( pid < 0 ) {
		c_printf( "init, spawn() user F failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_G
	pid = spawn( user_g );
	if( pid < 0 ) {
		c_printf( "init, spawn() user G failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_H
	pid = spawn( user_h );
	if( pid < 0 ) {
		c_printf( "init, spawn() user H failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_J
	pid = spawn( user_j );
	if( pid < 0 ) {
		c_printf( "init, spawn() user J failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_K
	pid = spawn( user_k );
	if( pid < 0 ) {
		c_printf( "init, spawn() user K failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_L
	pid = spawn( user_l );
	if( pid < 0 ) {
		c_printf( "init, spawn() user L failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_M
	pid = spawn( user_m );
	if( pid < 0 ) {
		c_printf( "init, spawn() user M failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_N
	pid = spawn( user_n );
	if( pid < 0 ) {
		c_printf( "init, spawn() user N failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_P
	pid = spawn( user_p );
	if( pid < 0 ) {
		c_printf( "init, spawn() user P failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_Q
	pid = spawn( user_q );
	if( pid < 0 ) {
		c_printf( "init, spawn() user Q failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_R
	pid = spawn( user_r );
	if( pid < 0 ) {
		c_printf( "init, spawn() user R failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_S
	pid = spawn( user_s );
	if( pid < 0 ) {
		c_printf( "init, spawn() user S failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_T
	pid = spawn( user_t );
	if( pid < 0 ) {
		c_printf( "init, spawn() user T failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_U
	pid = spawn( user_u );
	if( pid < 0 ) {
		c_printf( "init, spawn() user U failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_V
	pid = spawn( user_v );
	if( pid < 0 ) {
		c_printf( "init, spawn() user V failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_NULLPTR
	pid = spawn( user_nullptr );
	if( pid < 0 ) {
		c_printf( "init, spawn() user NULLPTR failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_BELOWSTACKS
	pid = spawn( user_belowstacks );
	if( pid < 0 ) {
		c_printf( "init, spawn() user BELOWSTACKS failed\n" );
		exit();
	}
#endif

#ifdef SPAWN_ABOVEUSER
	pid = spawn( user_aboveuser );
	if( pid < 0 ) {
		c_printf( "init, spawn() user ABOVEUSER failed\n" );
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
