/*
** SCCS ID:	%W%	%G%
**
** File:	user.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	User routines
*/

#ifndef _USER_H_
#define _USER_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// delay loop counts

#define	DELAY_LONG	100000000
#define	DELAY_STD	  2500000
#define	DELAY_ALT	  4500000

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** User process controls.
**
** The comment field of these definitions contains a list of the
** system calls this user process uses.
**
** To spawn a specific user process from the initial process,
** uncomment its entry in this list.
*/

//	user	          baseline system calls in use
		// xit rch wch ws  spn slp gpr spr gp  gpp gt
#define	SPAWN_A	//  X   .   X   .   .   .   .   .   .   .   .
#define	SPAWN_B	//  X   .   X   .   .   .   .   .   .   .   .
#define	SPAWN_C	//  X   .   X   .   .   .   .   .   .   .   .
#define	SPAWN_D	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_E	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_F	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_G	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_H	//  .   .   .   .   .   .   .   .   .   .   .
// no user i
#define	SPAWN_J	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_K	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_L	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_M	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_N	//  .   .   .   .   .   .   .   .   .   .   .
// no user O
#define	SPAWN_P	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_Q	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_R	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_S	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_T	//  .   .   .   .   .   .   .   .   .   .   .
#define	SPAWN_U	//  .   .   .   .   .   .   .   .   .   .   .
// no user V

/*
** Users W-Z are spawned from other processes; they
** should never be spawned directly.
*/

// user W:	    .   .   .   .   .   .   .   .   .   .   .
// user X:	    .   .   .   .   .   .   .   .   .   .   .
// user Y:	    .   .   .   .   .   .   .   .   .   .   .
// user Z:	    .   .   .   .   .   .   .   .   .   .   .


/*
** Prototypes for externally-visible routines
*/

/*
** init - initial user process
**
** after spawning the other user processes, loops forever calling wait()
*/

void init( void );

#endif

#endif
