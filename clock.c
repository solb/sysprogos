/*
** SCCS ID:	%W%	%G%
**
** File:	clock.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Clock module implementation
*/

#define	__SP_KERNEL__

#include "common.h"

#include <x86arch.h>
#include "startup.h"

#include "clock.h"
#include "process.h"
#include "queue.h"
#include "scheduler.h"
#include "sio.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// pinwheel control variables

static uint32_t _pinwheel;	// pinwheel counter
static uint32_t _pindex;	// index into pinwheel string

/*
** PUBLIC GLOBAL VARIABLES
*/

time_t	_system_time;		// the current system time

/*
** PRIVATE FUNCTIONS
*/


/*
** _clock_isr(vector,code)
**
** Interrupt handler for the clock module.  Spins the pinwheel,
** wakes up sleeping processes, and handles quantum expiration
** for the current process.
*/

static void _clock_isr( int vector, int code ) {
	pcb_t *pcb;

	// spin the pinwheel

	++_pinwheel;
	if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
		_pinwheel = 0;
		++_pindex;
		c_putchar_at( 79, 0, "|/-\\"[ _pindex & 3 ] );
	}

	// increment the system time

	++_system_time;

	/*
	** wake up any sleeper whose time has come
	**
	** we give awakened processes preference over the
	** current process (when it is scheduled again)
	*/

	while( _que_length(&_sleeping) != 0 ) {
		// peek at the first sleeper
		pcb = (pcb_t *) _que_peek( &_sleeping );
		if( PCB == NULL ) {
			_kpanic( "_clock_isr",
			    "NULL from non-empty sleep queue",
			    EMPTY_QUEUE );
		}
		// if it's time to wake it up, do so
		if( pcb->wakeup <= _system_time ) {
			// remove from the queue
			pcb = (pcb_t *) _que_remove( &_sleeping );
			if( pcb == NULL ) {
				_kpanic( "_clock_isr",
				    "NULL from sleep queue remove",
				    EMPTY_QUEUE );
			}
			// and schedule it for dispatch
			_schedule( pcb );
		} else {
			// we're done - no more to wake up
			break;
		}
	}
	
	// check the current process to see if it needs to be scheduled

	// sanity check!

	if( _current->quantum == 0 ) {
		_kpanic( "_clock_isr", "zero quantum on current process",
			FAILURE );
	}
	
	_current->quantum -= 1;
	if( _current->quantum == 1 ) {
		_schedule( _current );
		_dispatch();
	}

#ifdef DUMP_QUEUES
	// Approximately every 20 seconds, dump the queues, and
	// print the contents of the SIO buffers.

	if( (_system_time % seconds_to_ticks(20)) == 0 ) {
		c_printf( "Queue contents @%08x\n", _system_time );
		_que_dump( "ready[0]", &_ready[0] );
		_que_dump( "ready[1]", &_ready[1] );
		_que_dump( "ready[2]", &_ready[2] );
		_que_dump( "sleep", &_sleeping );
		_que_dump( "read", &_reading );
		_que_dump( "wait", &_waiting );
		_sio_dump();
	}
#endif

	// tell the PIC we're done

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}

/*
** PUBLIC FUNCTIONS
*/

/*
** _clock_init()
**
** initialize the clock module
*/

void _clock_init( void ) {
	uint32_t divisor;

	// start the pinwheel

	_pinwheel = _pindex = 0;

	// return to the epoch

	_system_time = 0;

	// set the clock to tick at CLOCK_FREQUENCY Hz.

	divisor = TIMER_FREQUENCY / CLOCK_FREQUENCY;
	__outb( TIMER_CONTROL_PORT, TIMER_0_LOAD | TIMER_0_SQUARE );
        __outb( TIMER_0_PORT, divisor & 0xff );        // LSB of divisor
        __outb( TIMER_0_PORT, (divisor >> 8) & 0xff ); // MSB of divisor

	// register the ISR

	__install_isr( INT_VEC_TIMER, _clock_isr );

        // announce that we have initialized the clock module

        c_puts( " clock" );
}
