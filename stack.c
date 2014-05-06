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
#include "memory.h"

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
	// No-op!
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

physaddr_t _stack_alloc( void ) {
	physaddr_t physical = _mem_page_frame_alloc();
	void *mapped = _mem_map_page( physical );
	_kmemclr( mapped, PAGE_SIZE );
	_mem_unmap_page( mapped );
	return physical;
}

/*
** _stack_free(stack)
**
** deallocate a stack, putting it into the list of available stacks
*/

void _stack_free( physaddr_t stack ) {
	_mem_page_frame_free( stack );
}
