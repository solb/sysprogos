/*
** SCCS ID:	@(#)process.h	1.1	4/9/14
**
** File:	process.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Process-related declarations
*/

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// number of user pcbs

#define	N_PCBS		N_PROCESSES

// process states

#define	FREE		0
#define	NEW		1
#define	READY		2
#define	RUNNING		3
#define	SLEEPING	4
#define	BLOCKED		5

#define	N_STATES	6

// process priorities

#define	PRIO_HIGH	0
#define	PRIO_STD	1
#define	PRIO_LOW	2

#define	PRIO_DEFAULT	PRIO_STD

#define	N_PRIOS		3
#define	PRIO_LAST	PRIO_LOW

// PID of the initial user process

#define	PID_INIT	1

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

#include "clock.h"
#include "stack.h"

// ARGn(p) - access argument #n register from process context
//
// IF THE PARAMETER PASSING MECHANISM CHANGES, SO MUST THESE

#define	ARG1(pcb)	((pcb)->context->ecx)
#define	ARG2(pcb)	((pcb)->context->edx)

// RET(p) - access return value register in process context

#define RET(p)  ((p)->context->eax)

/*
** Types
*/

// process state
typedef uint8_t		state_t;

// process quantum
typedef uint8_t		quantum_t;

// process context structure
//
// NOTE:  the order of data members here depends on the
// register save code in isr_stubs.S!!!!

typedef struct context {
	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t fake_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t vector;
	uint32_t code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
} context_t;

// process control block
//
// members are ordered by size

typedef struct pcb {
	// 32-bit fields
	context_t	*context;	// context save area pointer
	stack_t		*stack;		// per-process runtime stack
	time_t		wakeup;		// for sleeping process
	// 16-bit fields
	pid_t		pid;		// our pid
	pid_t		ppid;		// out parent's pid
	// 8-bit fields
	prio_t		prio;		// our priority (MLQ level)
	state_t		state;		// current process state
	quantum_t	quantum;	// remaining execution quantum
} pcb_t;

/*
** Globals
*/

extern pcb_t _pcbs[];		// all PCBs in the system
extern pid_t _next_pid;		// next available PID

/*
** Prototypes
*/

/*
** _pcb_init()
**
** initializes all pcb-related data structures
*/

void _pcb_init( void );

/*
** _pcb_alloc()
**
** allocate a pcb structure
**
** returns a pointer to the pcb, or NULL on failure
*/

pcb_t *_pcb_alloc( void );

/*
** _pcb_free(pcb)
**
** deallocate a pcb, putting it into the list of available pcbs
*/

void _pcb_free( pcb_t *pcb );

/*
** _pcb_dump(pcb)
**
** dump the contents of this PCB to the console
*/

void _pcb_dump( const char *which, pcb_t *pcb );

void _context_dump( const char *which, context_t *context );

#endif

#endif
