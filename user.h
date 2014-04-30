/*
** SCCS ID:	@(#)user.h	1.1	4/9/14
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
		// xit rch wch ws  spn slp gpr spr gp  gpp gt bog
#if 0
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
#define	SPAWN_R	//  X   X   X   .   .   X   .   .   .   .   .  .
#define	SPAWN_S	//  X   .   X   .   .   X   .   .   .   .   .  .
#define	SPAWN_T	//  X   .   X   .   .   .   X   X   X   .   .  .
#define	SPAWN_U	//  X   .   .   X   .   .   .   .   .   .   .  .
// no user V
#endif

#if 0
#define SPAWN_NULLPTR
#define SPAWN_BELOWSTACKS
#define SPAWN_ABOVEUSER
#endif

/*
** Users W-Z are spawned from other processes; they
** should never be spawned directly.
*/

// user W:	    X   .   X   .   .   X   .   .   X   .   .  .
// user X:	    X   .   X   .   .   X   .   .   X   X   .  .
// user Y:	    X   .   X   .   .   X   .   .   .   .   .  .
// user Z:	    X   .   X   .   .   .   .   .   X   X   .  .


/*
** Prototypes for externally-visible routines
*/

/*
** init - initial user process
**
** after spawning the other user processes, becomes the idle process
*/

void init( void );

#endif

#endif
