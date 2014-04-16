/*
** SCCS ID:	@(#)system.c	1.1	4/9/14
**
** File:	system.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Miscellaneous OS support implementations
*/

#define	__SP_KERNEL__

#include "common.h"

#include "system.h"
#include "clock.h"
#include "process.h"
#include "bootstrap.h"
#include "syscall.h"
#include "sio.h"
#include "scheduler.h"

// need init() address
#include "user.h"

// need the exit() prototype
#include "ulib.h"

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

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _create_process(ppid,entry)
**
** allocate and initialize a new process' data structures (PCB, stack)
**
** returns:
**      pointer to the new PCB
*/

pcb_t *_create_process( pid_t ppid, uint32_t entry ) {
	pcb_t *new;
	uint32_t *ptr;
	
	// allocate the new structures

	new = _pcb_alloc();
	if( new == NULL ) {
		return( NULL );
	}

	// clear all the fields in the PCB

	_kmemclr( (void *) new, sizeof(pcb_t) );

	new->stack = _stack_alloc();
	if( new->stack == NULL ) {
		_pcb_free( new );
		return( NULL );
	}

	// clear the stack

	_kmemclr( (void *) new->stack, sizeof(stack_t) );

	/*
	** We need to set up the initial stack contents for the new
	** process.  The low end of the initial stack must look like this:
	**
	**      esp ->  ?     <- context save area
	**              ...   <- context save area
	**              ?     <- context save area
	**              exit  <- return address for faked call to main()
	**              0     <- last word in stack
	*/

	// first, create a pointer to the longword after the stack

	ptr = (uint32_t *) (new->stack + 1);

	// save the buffering 0 at the end

	*--ptr = 0;

	// fake a return address so that if the user function returns
	// without calling exit(), we return "into" exit()

	*--ptr = (uint32_t) exit;

	uint32_t *top_stack = ptr;

	// locate the context save area

	new->context = ((context_t *) ptr) - 1;

	// fill in the non-zero entries in the context save area

	new->context->eip    = entry;
	new->context->cs     = GDT_CODE;
	new->context->ss     = GDT_STACK;
	new->context->ds     = GDT_DATA;
	new->context->es     = GDT_DATA;
	new->context->fs     = GDT_DATA;
	new->context->gs     = GDT_DATA;
	new->context->eflags = DEFAULT_EFLAGS;
	new->context->esp    = top_stack;

	// fill in the remaining important fields

	new->pid  = _next_pid++;
	new->ppid = ppid;
	new->prio = PRIO_DEFAULT;
	
	// all done - return the new PCB

	return( new );
}

/*
** _init - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/

void _init( void ) {
	pcb_t *pcb;

	/*
	** BOILERPLATE CODE - taken from basic framework
	**
	** Initialize interrupt stuff.
	*/

	__init_interrupts();	// IDT and PIC initialization

	/*
	** Console I/O system.
	*/

	c_io_init();
	c_setscroll( 0, 7, 99, 99 );
	c_puts_at( 0, 6, "================================================================================" );

	/*
	** 20135-SPECIFIC CODE STARTS HERE
	*/

	/*
	** Initialize various OS modules
	**
	** Note:  the clock, SIO, and syscall modules also install
	** their ISRs.
	*/

	c_puts( "Module init: " );

	_que_init();		// must be first
	_pcb_init();
	_stack_init();
	_sched_init();
	_sio_init();
	_sys_init();
	_clock_init();

	c_puts( "\n" );

	/*
	** Create the initial system ESP and populate the TSS
	**
	** The former will be the address of the next-to-last
	** longword in the system stack.
	*/

	_system_esp = ((uint32_t *) ( (&_system_stack) + 1)) - 2;
	_stack_mktss();

	/*
	** Create the initial process
	**
	** Code mostly stolen from _sys_fork(); if that routine
	** changes, SO MUST THIS!!!
	*/

	// allocate a PCB and stack

	pcb = _create_process( 0, (uint32_t) init );
	if( pcb == NULL ) {
		_kpanic( "_init", "init() creation failed", FAILURE );
	}

	_pcb_dump( "initial", pcb );
	_context_dump( "initial", pcb->context );

	// make this the first process

	_schedule( pcb );
	_dispatch();

	/*
	** Turn on the SIO receiver (the transmitter will be turned
	** on/off as characters are being sent)
	*/

	_sio_enable( SIO_RX );

	/*
	** END OF 20135-SPECIFIC CODE
	**
	** Finally, report that we're all done.
	*/

	c_puts( "System initialization complete.\n" );

}
