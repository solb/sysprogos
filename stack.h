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

// user stack

typedef uint32_t	stack_t[ STACK_SIZE ];

/*
** Globals
*/

extern uint32_t *_system_esp;		// OS stack pointer

/*
** Prototypes
*/

/*
** _stack_init()
**
** initializes all stack-related data structures
*/

void _stack_init( void );

/*
** _stack_alloc()
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/

physaddr_t _stack_alloc( void );

/*
** Name:	_stack_mktss
**
** initialize the Task-Segment Selector (TSS)
*/
void _stack_mktss( void );

/*
** _stack_free(stack)
**
** deallocate a stack, putting it into the list of available stacks
*/

void _stack_free( physaddr_t stack );

#endif

#endif
