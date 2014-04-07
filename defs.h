/*
** SCCS ID:	%W%	%G%
**
** File:	defs.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	General system definitions
*/

#ifndef _DEFS_H_
#define _DEFS_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// Null pointer (defined the correct way :-)

#define	NULL		0

// Maximum number of simultaneous user processes

#define	N_PROCESSES	25

#ifdef __SP_KERNEL__

// kernel status values

#define	SUCCESS		0
#define	FAILURE		1
#define	BAD_PARAM	2
#define	EMPTY_QUEUE	3
#define	NOT_EMPTY_QUEUE	4
#define	NOT_FOUND	5
#define	ALLOC_FAILED	6

#define	N_STATUS	7

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

#include "types.h"

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

#endif

#endif

#endif
