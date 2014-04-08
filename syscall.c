/*
** SCCS ID:	%W%	%G%
**
** File:	syscall.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	System call module implementation
*/

#define	__SP_KERNEL__

#include "common.h"

#include "syscall.h"

#include "process.h"
#include "stack.h"
#include "queue.h"
#include "scheduler.h"
#include "sio.h"

#include "support.h"
#include "startup.h"
#include <x86arch.h>

/*
** PRIVATE DEFINITIONS
*/

// access to user-supplied arguments
//
// IF THE PARAMETER PASSING MECHANISM CHANGES, SO MUST THESE

#define	ARG1(pcb)	((pcb)->context->ecx)
#define	ARG2(pcb)	((pcb)->context->edx)

// access to the "return value" register save area

#define	RET(pcb)	((pcb)->context->eax)

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// system call jump table
//
// initialized by _sys_init() to ensure that
// code::function mappings are correct

static void (*_syscalls[ N_SYSCALLS ])( pcb_t *);

/*
** PUBLIC GLOBAL VARIABLES
*/

queue_t _sleeping;	// sleeping processes

/*
** PRIVATE FUNCTIONS
*/

/*
** _sys_isr(vector,code)
**
** Common handler for the system call module.  Selects
** the correct second-level routine to invoke based on
** the contents of EAX.
**
** The second-level routine is invoked with a pointer to
** the PCB for the process.  It is the responsibility of
** that routine to assign all return values for the call.
*/

static void _sys_isr( int vector, int code ) {
	uint32_t code= _current->context->eax;

	// verify that we were given a legal code

	if( code >= N_SYSCALLS ) {

		// nope - report it...

		c_printf( "*** _sys_isr, PID %d syscall %d\n",
			_current->pid, code );

		// ...and force it to exit()

		code = SYS_exit;
	}

	// invoke the appropriate syscall handler

	*(_syscalls[code])( _current );

	// tell the PIC we're done

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
}

/*
** Second-level syscall handlers
**
** All have this prototype:
**
**      static void _sys_NAME( pcb_t * );
**
** Those which return results to the calling user do so through the
** context save area pointed to by the supplied PCB.  This is also
** where incoming parameters to the syscall are found.
*/

/*
** _sys_getpid - retrieve the PID of the current process
**
** implements:	pid_t getpid(void);
**
** returns:
**	the process' pid
*/

static void _sys_getpid( pcb_t *pcb ) {
	RET(pcb) = pcb->pid;
}

/*
** _sys_getppid - retrieve the parent PID of the current process
**
** implements:	pid_t getppid(void);
**
** returns:
**	the process' parent's pid
*/

static void _sys_getppid( pcb_t *pcb ) {
	RET(pcb) = pcb->ppid;
}

/*
** _sys_gettime - retrieve the current system time
**
** implements:	time_t gettime(void);
**
** returns:
**	the current system time
*/

static void _sys_gettime( pcb_t *pcb ) {
	RET(pcb) = _system_time;
}

/*
** _sys_getprio - retrieve the priority of this process
**
** implements:	int getprio(void);
**
** returns:
**	the process' priority
*/

static void _sys_getprio( pcb_t *pcb ) {
	RET(pcb) = pcb->prio;
}

/*
** _sys_setprio - change the priority of this process
**
** implements:	prio_t setprio(prio_t prio);
**
** returns:
**	the previous priority
*/

static void _sys_setprio( pcb_t *pcb ) {
	prio_t prio = ARG1(pcb);
		
	// verify that the priority is legal
	//
	// if it isn't, use the default priority

	if( prio > PRIO_LAST ) {
		prio = PRIO_DEFAULT;
	}

	// return the old priority value

	RET(pcb) = pcb->prio;

	// set the priority

	pcb->prio = prio;
}

/*
** _sys_exit - terminate the calling process
**
** implements:  void exit(void);
**
** does not return
*/

static void _sys_exit( pcb_t *pcb ) {
	
	// tear down the PCB structure

	_stack_free( pcb->stack );
	_pcb_free( pcb );

	// if this was the current process, we need a new one

	if( pcb == _current ) {
		_dispatch();
	}
}

/*
** _sys_readch - read a single character from the SIO
**
** implements:	int readch(void);
**
** blocks the calling routine if there is no character to return
**
** returns:
**	the character
*/

static void _sys_readch( pcb_t *pcb ) {
	int ch;

	// try to get the next character

	ch = _sio_readc();

	// if there was a character, return it to the process;
	// otherwise, block the process until one comes in

	if( ch >= 0 ) {

		RET(pcb) = ch & 0xff;

	} else {

		// no character; put this process on the
		// serial i/o input queue

		_current->state = BLOCKED;

		_que_insert( &_reading, (void *) _current );

		// select a new current process

		_dispatch();

	}

}

/*
** _sys_writech - write a single character to the SIO
**
** implements:	int writech(char buf);
**
** returns:
**	the character that was written
*/

static void _sys_writech( pcb_t *pcb ) {
	int ch = ARG1(pcb) & 0xff;

	// this is almost insanely simple, but it does separate
	// the low-level device access fromm the higher-level
	// syscall implementation

	_sio_writec( ch );

	RET(pcb) = ch;

}

/*
** _sys_writes - write a NUL-terminated string to the SIO
**
** implements:	int writes(char *buf);
**
** returns:
**	the number of characters that were sent
*/

static void _sys_writes( pcb_t *pcb ) {
	char *str = ARG1(pcb);

	// this is almost insanely simple, but it does separate
	// the low-level device access fromm the higher-level
	// syscall implementation

	RET(pcb) = _sio_writes( str );

}

/*
** _sys_spawn - create a new process running the specified program
**
** implements:  int spawn(void (*entry)(void));
**
** returns:
**	pid of new process in original process, or -1 on error
*/

static void _sys_spawn( pcb_t *pcb ) {
	pcb_t *new;

	// farm out all the work to this supporting routine

	new = _create_process( pcb->pid, ARG1(pcb) );

	if( new != NULL ) {

		// it succeeded - tell the parent and schedule the child
		RET(pcb) = new->pid
		_schedule( new );

	} else {

		// it failed - tell the parent
		RET(pcb) = -1;

	}
}


/*
** _sys_sleep - put the current process to sleep for some length of time
**
** implements:	void sleep(uint32_t ms);
**
** If the sleep time (in milliseconds) is 0, just preempts the process;
** otherwise, puts it onto the sleep queue for the specified length of
** time.
*/

static void _sys_sleep( pcb_t *pcb ) {
	uint_t sleeptime = ARG1(pcb);
	
	// if the desired sleep time is 0, treat this as a yield()

	if( sleeptime == 0 ) {

		// just preempt the process
		_schedule( pcb );

	} else {

		// calculate the wakeup time for the process
		pcb->wakeup = _system_time + ms_to_ticks(sleeptime);

		// mark it as sleeping
		pcb->state = SLEEPING;

		// add it to the sleep queue
		_que_insert( &_sleeping, (void *)pcb );

	}
	
	// no current process - pick another one

	_dispatch();
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _sys_init()
**
** initialize the syscall module
*/

void _sys_init( void ) {

	// initialize the sleep queue

	_que_reset( &_sleeping, _compare_time );

	/*
	** Set up the syscall jump table.  We do this here
	** to ensure that the association between syscall
	** code and function address is correct even if the
	** codes change.
	*/

	_syscalls[ SYS_exit ]      = _sys_exit;
	_syscalls[ SYS_readch ]    = _sys_readch;
	_syscalls[ SYS_writech ]   = _sys_writech;
	_syscalls[ SYS_writes ]    = _sys_writes;
	_syscalls[ SYS_spawn ]     = _sys_spawn;
	_syscalls[ SYS_sleep ]     = _sys_sleep;
	_syscalls[ SYS_getprio ]   = _sys_getprio;
	_syscalls[ SYS_setprio ]   = _sys_setprio;
	_syscalls[ SYS_getpid ]    = _sys_getpid;
	_syscalls[ SYS_getppid ]   = _sys_getppid;
	_syscalls[ SYS_gettime ]   = _sys_gettime;

	// install our ISR

	__install_isr( INT_VEC_SYSCALL, _sys_isr );

	c_puts( " syscall" );
}
