/*
** SCCS ID:	@(#)queue.h	1.1	4/9/14
**
** File:	queue.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Queue module declarations
*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

// Pseudo-function:  length of a queue
#define _que_length(qp)	((qp)->size)

/*
** Types
**
** Our queues are generic, self-ordering queues.  The queue itself
** is a doubly-linked list of link nodes, which in turn point to
** the information queued up in this position.  Each queue has an
** associated ordering routine which is used by the insertion 
** routine to order the queue elements.  This allows us to have
** different sorting criteria for different queues, but manage them
** with one set of functions.
*/

// link node

typedef struct link {
	void *data;		// what's queued up here
	struct link *next;	// "forward" link
} linknode_t;

// queue header

typedef struct queue {
	linknode_t *first;		// first entry
	linknode_t *last;		// last entry
	uint32_t size;			// number of entries
	int (*compare)( void *one, void *two );	// ordering routine
} queue_t;

/*
** Globals
*/

/*
** Prototypes
*/

/*
** compare two PCBs using the wakeup time
**
** returns the status of the comparison:
**      < 0     p1 < p2
**      = 0     p1 == p2
**      > 0     p1 > p2
*/

int _compare_time( register void *i1, register void *i2 );

/*
** initialize the queue module
*/

void _que_init( void );

/*
** reset the supplied queue by setting its link pointers to NULL
** and associating the supplied comparison routine with it
*/

void _que_reset( queue_t *que, int (*compare)(void*,void*) );

/*
** insert the supplied data value into the queue, ordering the
** queue using its built-in comparison function
*/

void _que_insert( queue_t *queue, void *data );

/*
** remove the first element from the queue
**
** returns the thing that was removed, or NULL
*/

void *_que_remove( queue_t *que );

/*
** peek at the first element in the queue
**
** returns a pointer to the first queued data item, or NULL
*/

void *_que_peek( queue_t *que );

/*
** dump the contents of the specified queue to the console
*/

void _que_dump( char *which, queue_t *queue );

#endif

#endif
