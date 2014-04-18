/*
** SCCS ID:	@(#)stack.c	1.1	4/9/14
**
** File:	stack.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Stack module implementation
*/

#define	__SP_KERNEL__

#include "bootstrap.h"
#include "common.h"

#include "stack.h"
#include "queue.h"

#include "klib.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static stack_t _stacks[ N_STACKS ];	// user process stacks
static queue_t _free_stacks;		// list of available stacks

/*
** PUBLIC GLOBAL VARIABLES
*/

stack_t _system_stack;			// stack for the OS
uint32_t *_system_esp;			// OS stack pointer

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _stack_init()
**
** initializes all stack-related data structures
*/

void _stack_init( void ) {
	int i;
	
	// clear the free stack queue

	_que_reset( &_free_stacks, NULL );
	
	// "free" all the stacks

	for( i = 0; i < N_STACKS; ++i ) {
		_stack_free( &_stacks[i] );
	}
	
	// report that we have finished

	c_puts( " stacks" );
}

/*
** Name:	_stack_mktss
**
** Description: Initialize the Task-Segment Selector (TSS) so that
** 		we'll be able to jump from ring 3 back to ring 0.
*/
void _stack_mktss( void ){
	uint32_t *tss = (uint32_t *)TSS_ADDRESS;
	_kmemclr((byte_t *)TSS_ADDRESS, TSS_SIZE);
	tss[TSS_ESP0] = (uint32_t)_system_esp;
	__inst_tss();
}

/*
** _stack_alloc()
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/

stack_t *_stack_alloc( void ) {
	// pull the first available stack off the free queue
	return( (stack_t *) _que_remove( &_free_stacks ) );
}

/*
** _stack_free(stack)
**
** deallocate a stack, putting it into the list of available stacks
*/

void _stack_free( stack_t *stack ) {
	
	// sanity check:  avoid deallocating a NULL pointer
	if( stack == NULL ) {
		// this should probably be an error
		return;
	}

	// return the stack to the free list

	_que_insert( &_free_stacks, (void *) stack );
}
