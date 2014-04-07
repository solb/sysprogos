/*
** SCCS ID:	%W%	%G%
**
** File:	clock.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Clock module
*/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// clock interrupts per second

#define	CLOCK_FREQUENCY		1000

// standard process quantum (in ticks)

#define	QUANTUM_DEFAULT		10

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

// pseudo function to convert seconds to milliseconds

#define	seconds_to_ms(n)	((n) * 1000)

// pseudo function to convert milliseconds to clock ticks
// (currently, a no-op, as the base clock rate is 1000 ticks/sec)

#define	ms_to_ticks(n)		((n))

// pseudo function to convert seconds to ticks

#define	seconds_to_ticks(n)	(seconds_to_ms((n)) * CLOCK_FREQUENCY)

// pseudo function to convert ticks to (truncated) seconds

#define	ticks_to_seconds(n)	((n) / CLOCK_FREQUENCY)

// pseudo function to convert ticks to (rounded up) seconds

#define	ticks_to_rounded_seconds(n)	(((n)+(CLOCK_FREQUENCY-1)) / CLOCK_FREQUENCY)

/*
** Types
*/

/*
** Globals
*/

extern time_t	_system_time;	// the current system time

/*
** Prototypes
*/

/*
** _clock_init()
**
** initialize the clock module
*/

void _clock_init( void );

#endif

#endif
