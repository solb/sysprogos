/*
** SCCS ID:	@(#)stack.h	1.1	4/9/14
**
** File:	stack.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Stack module declarations
*/

#ifndef _STACK_H_
#define _STACK_H_

#define __SP_KERNEL__

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// stack size (in longwords)

#define	STACK_SIZE	1024

// number of user stacks

#define	N_STACKS	N_PROCESSES

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

extern uint32_t *_system_esp;		// OS stack pointer

/*
** Prototypes
*/

/*
** initializes all stack-related data structures
*/

void _stack_init( void );

/*
** allocate a stack structure
**
** returns a pointer to the stack
*/

physaddr_t _stack_alloc( void );

/*
** initialize the Task-Segment Selector (TSS) so that
** we'll be able to jump from ring 3 back to ring 0.
*/
void _stack_mktss( void );

/*
** deallocate a stack, freeing its page frame
*/

void _stack_free( physaddr_t stack );

#endif

#endif
