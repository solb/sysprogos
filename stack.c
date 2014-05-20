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

uint32_t *_system_esp;			// OS stack pointer

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

void _stack_init( void ) {
	static const int SYSTEM_STACK_PAGE = 0x10e000;
	//Init system stack:
	physaddr_t stack_loc = _mem_page_frame_alloc();
	_mem_map_page_onto(SYSTEM_STACK_PAGE, stack_loc);
	_system_esp = SYSTEM_STACK_PAGE + PAGE_SIZE;
}

void _stack_mktss( void ){
	uint32_t *tss = (uint32_t *)TSS_ADDRESS;
	_kmemclr((byte_t *)TSS_ADDRESS, TSS_SIZE);

	// Selects the stack to switch to when receiving interrupts from userland
	tss[TSS_RSP0] = (uint32_t)_system_esp;

	// Selects the stack to switch to when receiving supervisory interrupts
	tss[TSS_IST1] = (uint32_t)_system_esp;

	// We need to disable the I/O permissions bit vector to ban ins and outs
	tss[TSS_IOBM] = 0xffff << 16;

	__inst_tss();
}

physaddr_t _stack_alloc( void ) {
	physaddr_t physical = _mem_page_frame_alloc();
	void *mapped = _mem_map_page( physical );
	_kmemclr( mapped, PAGE_SIZE );
	_mem_unmap_page( mapped );
	return physical;
}

void _stack_free( physaddr_t stack ) {
	_mem_page_frame_free( stack );
}
