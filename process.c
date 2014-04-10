/*
** SCCS ID:	@(#)process.c	1.1	4/9/14
**
** File:	process.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Process-related implementations
*/

#define	__SP_KERNEL__

#include "common.h"

#include "process.h"
#include "queue.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _free_pcbs;	// queue of available PCBs

/*
** PUBLIC GLOBAL VARIABLES
*/

pcb_t _pcbs[ N_PCBS ];		// all the PCBs in the system
pid_t _next_pid;		// next available PID

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _pcb_init()
**
** initializes all pcb-related data structures
*/

void _pcb_init( void ) {
	int i;
	
	// clear the free PCB queue

	_que_reset( &_free_pcbs, NULL );
	
	// "free" all the PCBs

	for( i = 0; i < N_PCBS; ++i ) {
		_pcb_free( &_pcbs[i] );
	}
	
	// set the initial PID
	// (must be the PID of the init() process)

	_next_pid = PID_INIT;

	// report that we have finished

	c_puts( " pcb" );
}

/*
** _pcb_alloc()
**
** allocate a pcb structure
**
** returns a pointer to the pcb, or NULL on failure
*/

pcb_t *_pcb_alloc( void ) {
	// pull the first available PCB off the free queue
	return( (pcb_t *) _que_remove( &_free_pcbs ) );
}

/*
** _pcb_free(pcb)
**
** deallocate a pcb, putting it into the set of available pcbs
*/

void _pcb_free( pcb_t *pcb ) {
	
	// sanity check:  avoid deallocating a NULL pointer
	if( pcb == NULL ) {
		// should this be an error?
		return;
	}

	// return the PCB to the free list

	pcb->state = FREE;
	_que_insert( &_free_pcbs, (void *) pcb );
}

/*
** _pcb_dump(pcb)
**
** dump the contents of this PCB to the console
*/

void _pcb_dump( const char *which, pcb_t *pcb ) {

	c_printf( "%s: ", which );
	if( pcb == NULL ) {
		c_puts( " NULL???\n" );
		return;
	}

	c_printf( " pids %d/%d state ", pcb->pid, pcb->ppid );
	switch( pcb->state ) {
		case FREE:	c_puts( "FREE" ); break;
		case NEW:	c_puts( "NEW" ); break;
		case READY:	c_puts( "READY" ); break;
		case RUNNING:	c_puts( "RUNNING" ); break;
		case SLEEPING:	c_puts( "SLEEPING" ); break;
		case BLOCKED:	c_puts( "BLOCKED" ); break;
		default:	c_printf( "? (%d)", pcb->state );
	}

	c_puts( " prio " );
	switch( pcb->prio ) {
		case PRIO_HIGH:		c_puts( "HIGH" ); break;
		case PRIO_STD:		c_puts( "STD" ); break;
		case PRIO_LOW:		c_puts( "LOW" ); break;
		default:	c_printf( "? (%d)", pcb->prio );
	}

	c_printf( "\n q %d wake %08x", pcb->quantum, pcb->wakeup );

	c_printf( " context %08x stack %08x\n",
		  (uint32_t) pcb->context, (uint32_t) pcb->stack );
}

/*
** _context_dump(context)
*/

void _context_dump( const char *which, context_t *context ) {

	c_printf( "%s: ", which );
	if( context == NULL ) {
		c_puts( " NULL???\n" );
		return;
	}

	c_printf( "\n\t ss %08x  gs %08x  fs %08x  es %08x\n",
		context->ss, context->gs, context->fs, context->es );
	c_printf( "\t ds %08x edi %08x esi %08x ebp %08x\n",
		context->ds, context->edi, context->esi, context->ebp );
	c_printf( "\tesp %08x ebx %08x edx %08x ecx %08x\n",
		context->esp, context->ebx, context->edx, context->ecx );
	c_printf( "\teax %08x vec %08x cod %08x eip %08x\n",
		context->eax, context->vector, context->code, context->eip );
	c_printf( "\t cs %08x efl %08x\n",
		context->cs, context->eflags );

}