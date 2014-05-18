/*
** Author:	Sol Boucher
**
** Description: Userspace test suite header
*/

#ifndef TESTS_H_
#define TESTS_H_

#include "userspace.h"

/*
** Test process controls.
**
** The comment field of these definitions contains a list of the
** system calls this user process uses.
**
** To spawn a specific user process from the initial process,
** uncomment its entry in this list.
*/

//	user	          baseline system calls in use
		// xit rch wch ws  spn slp gpr spr gp  gpp gt bog
#define	SPAWN_A	//  X   .   X   .   .   .   .   .   .   .   .  .
#define	SPAWN_B	//  X   .   X   .   .   .   .   .   .   .   .  .
#define	SPAWN_C	//  X   .   X   .   .   .   .   .   .   .   .  .
// #define	SPAWN_D	//  X   .   X   .   X   .   .   .   .   .   .  .
#define	SPAWN_E	//  X   .   X   .   .   X   .   .   X   .   .  .
#define	SPAWN_F	//  X   .   X   .   .   X   .   .   X   .   .  .
#define	SPAWN_G	//  X   .   X   .   .   X   .   .   X   .   .  .
#define	SPAWN_H	//  .   .   X   .   .   .   .   .   .   .   .  .
// no user i
#define	SPAWN_J	//  X   .   X   .   X   .   .   .   .   .   .  .
#define	SPAWN_K	//  X   .   X   .   X   X   .   .   .   .   .  .
#define	SPAWN_L	//  X   .   X   .   X   X   .   .   .   .   X  .
#define	SPAWN_M	//  X   .   X   .   .   .   .   X   .   .   .  .
#define	SPAWN_N	//  X   .   X   .   .   .   .   X   .   .   .  .
// no user O
#define	SPAWN_P	//  X   .   X   .   .   X   .   .   .   .   X  .
#define	SPAWN_Q	//  X   .   X   .   .   .   .   .   .   .   .  X
#if 0
#define	SPAWN_R	//  X   X   X   .   .   X   .   .   .   .   .  .
#define	SPAWN_S	//  X   .   X   .   .   X   .   .   .   .   .  .
#endif
#define	SPAWN_T	//  X   .   X   .   .   .   X   X   X   .   .  .
#define	SPAWN_U	//  X   .   .   X   .   .   .   .   .   .   .  .
// no user V

// Protection checks
#define SPAWN_NULLPTR
#define SPAWN_OVERFLOWSTACK
#define SPAWN_UNDERFLOWSTACK
#define SPAWN_STOPPIC

/*
** Users W-Z are spawned from other processes; they
** should never be spawned directly.
*/

// user W:	    X   .   X   .   .   X   .   .   X   .   .  .
// user X:	    X   .   X   .   .   X   .   .   X   X   .  .
// user Y:	    X   .   X   .   .   X   .   .   .   .   .  .
// user Z:	    X   .   X   .   .   .   .   .   X   X   .  .

#endif
