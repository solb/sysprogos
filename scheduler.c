/*
** SCCS ID:	@(#)scheduler.c	1.1	4/9/14
**
** File:	scheduler.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:	Sol Boucher
**
** Description:	Scheduler/dispatcher implementation
*/

#define	__SP_KERNEL__

#include "common.h"

#include "scheduler.h"
#include "memory.h"

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

pcb_t *_current;		// the currently-running process
queue_t _ready[N_READYQ];	// the MLQ ready queue structure

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

void _sched_init( void ) {
	
	// reset all the MLQ levels

	for( int i = 0; i < N_READYQ; ++i ) {
		_que_reset( &_ready[i], NULL );
	}
	
	// no current process, initially

	_current = NULL;
	
	// report that we have finished

	c_puts( " sched" );
}

void _schedule( pcb_t *pcb ) {
	
	// sanity check:  verify that we don't have a NULL pointer?

	if( pcb == NULL ) {
		_kpanic( "_schedule", "NULL pcb pointer", BAD_PARAM );
	}

	// ensure the priority value for this process is legal

	if( pcb->prio >= N_PRIOS ) {
		pcb->prio = PRIO_LOW;
	}
	
	// mark this process as ready

	pcb->state = READY;

	// add it to the appropriate ready queue level

	_que_insert( &_ready[pcb->prio], (void *)pcb );
}

void _dispatch( void ) {
	pcb_t *new;
	
	// select a process from the highest-priority
	// ready queue that is not empty

	for( int i = 0; i < N_READYQ; ++i ) {

		if( _que_length(&_ready[i]) != 0 ) {
			new = (pcb_t *) _que_remove( &_ready[i] );
			if( new != NULL ) {
				_current = new;
				_mem_map_user_pagetab( _current->pagetab );
				_current->quantum = QUANTUM_DEFAULT;
				_current->state = RUNNING;
				return;
			}
		}
	}

	// uh, oh - didn't find one

	_kpanic( "_dispatch", "no non-empty ready queue", EMPTY_QUEUE );
}

void _terminate( void ) {
	if( _current->pid == PID_INIT )
		_kpanic( "_terminate", "attempted to kill init", FAILURE );

	// schedule the parent process if it was waiting for us to close

	if( _current->ppcb && _current->ppcb->state == BLOCKED ) {
		_schedule( _current->ppcb );
	}

	// tear down the PCB structure

	_mem_unmap_user_pagetab();
	_mem_page_table_free( _current->pagetab );
	_pcb_free( _current );

	// this was the current process, so we need a new one

	_dispatch();
}
