/*
** SCCS ID:	@(#)queue.c	1.1	4/9/14
**
** File:	queue.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Queue module implmentation
*/

#define	__SP_KERNEL__

#include "common.h"

#include "queue.h"

#include "process.h"
#include "stack.h"

/*
** PRIVATE DEFINITIONS
*/

// number of link nodes to allocate
//
// need one per PCB, one per stack, and a few extra

#define	N_LINKNODES	(N_PCBS + N_STACKS + 5)

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static linknode_t _linknodes[ N_LINKNODES ];	// the set of link nodes
static linknode_t *_free_linknodes;		// list of free link nodes

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** _link_alloc()
**
** allocate a link, returning a pointer to it or NULL
*/

linknode_t *_link_alloc( void ) {
	linknode_t *new;
	
	// pull the first one off the free list

	new = _free_linknodes;
	if( new != NULL ) {
		_free_linknodes = new->next;
	}
	
	// return it to the caller

	return( new );
}

/*
** _link_free(link)
**
** deallocate a link, putting it into the list of available links
*/

void _link_free( linknode_t *link ) {

	// sanity check:  don't deallocate a NULL pointer
	if( link == NULL ) {
		// this should probably be an error
		return;
	}

	// fastest method:  put this at the front of the list

	link->next = _free_linknodes;
	_free_linknodes = link;
}


/*
** PUBLIC FUNCTIONS
*/

int _compare_time( register void *p1, register void *p2 ) {

	// should really do a sanity check here, but what if
	// it fails?

	if( ((pcb_t *)p1)->wakeup < ((pcb_t *)p2)->wakeup )
		return( -1 );
	else if( ((pcb_t *)p1)->wakeup == ((pcb_t *)p2)->wakeup )
		return( 0 );

	return( 1 );

}

void _que_init( void ) {
	int i;
	
	// create the list of link nodes

	_free_linknodes = NULL;
	for( i = 0; i < N_LINKNODES; ++i ) {
		_link_free( &_linknodes[i] );
	}

	// report that we have finished

	c_puts( " queues" );
}

void _que_reset( queue_t *queue, int (*compare)(void*,void*) ) {

	// sanity check!

	if( queue == NULL ) {
		_kpanic( "_que_reset", "NULL queue parameter", BAD_PARAM );
	}

	// if there are any things in the queue, reclaim the linknodes

	for( linknode_t *curr = queue->first; curr != NULL; ) {
		linknode_t *tmp = curr;
		curr = curr->next;
		_link_free( tmp );
	}

	queue->first   = NULL;
	queue->last    = NULL;
	queue->size    = 0;
	queue->compare = compare;

}

void _que_insert( queue_t *queue, void *data ) {
	linknode_t *prev, *curr, *new;
	
	// sanity check
	//
	// note that inserting a NULL pointer is not prohibited

	if( queue == NULL ) {
		return;
	}
	
	// allocate a link for the insertion

	new = _link_alloc();
	if( new == NULL ) {
		_kpanic( "_que_insert", "linknode alloc failed", ALLOC_FAILED );
	}

	// copy the supplied data into the new link

	new->data = data;
	new->next = NULL;
	
	/*
	** handle the special cases of an empty queue and an unordered
	** queue here
	*/

	if( _que_length(queue) == 0 ) {
		queue->first = queue->last = new;
		queue->size = 1;
		return;
	}

	if( queue->compare == NULL ) {
		queue->last->next = new;
		queue->last = new;
		queue->size += 1;
		return;
	}
	
	/*
	** OK, we have a non-empty, ordered queue.  Locate the correct
	** insertion point.
	*/

	prev = NULL;
	curr = queue->first;
	while( curr != NULL ) {
		if( queue->compare(curr->data,new->data) > 0 ) {
			break;
		}
		prev = curr;
		curr = curr->next;
	}

	/*
	** Here are the possible outcomes of the search:
	**
	** prev	 curr	actions
	** ====	 ====	===================================
	**  0    !0	new->next = curr; // add at front
	**		first = new;
	**
	** !0    !0   	new->next = curr; // add in middle
	**		prev->next = new;
	**
	** !0    0   	new->next = curr; // add at end
	**		prev->next = new;
	**		last = new;
	*/

	// always

	new->next = curr;

	if( prev == NULL ) {
		// add at beginning
		queue->first = new;
	} else {
		// add after predecessor
		prev->next = new;
	}
	if( curr == NULL ) {
		queue->last = new;
	}

	queue->size += 1;
	
}

void *_que_remove( queue_t *queue ) {
	linknode_t *node;
	void *data;
	
	// sanity check!

	if( queue == NULL || _que_length(queue) == 0 ) {
		return( NULL );
	}
	
	// non-empty queue; remove the first element

	node = queue->first;

	if( node != NULL ) {

		// copy out the data pointer

		data = node->data;
	
		// unlink this from the list

		queue->first = node->next;
		if( queue->first == NULL ) {
			queue->last = NULL;
			queue->size = 0;
		} else {
			queue->size -= 1;
		}
	
		// release the Kraken..., uh, link

		_link_free( node );
	
	} else {

		data = NULL;

	}
	
	return( data );
}

void *_que_peek( queue_t *queue ) {
	
	// sanity check!

	if( queue == NULL || _que_length(queue) == 0 ) {
		return( NULL );
	}
	
	// non-empty queue; show the first element

	return( queue->first->data );
}

void _que_dump( char *which, queue_t *queue ) {
	linknode_t *tmp;
	int i;

	c_printf( "%s: ", which );
	if( queue == NULL ) {
		c_puts( "NULL???" );
		return;
	}

	c_printf( "first %08x last %08x comp %08x (%d items)\n",
		  (uint32_t) queue->first, (uint32_t) queue->last,
		  (uint32_t) queue->compare, queue->size );
	
	if( _que_length(queue) > 0 ) {
		c_puts( " data: " );
		i = 0;
		for( tmp = queue->first; tmp != NULL; tmp = tmp->next ) {
			c_printf( " [%x]", (uint32_t) tmp->data );
			if( ++i > 10 ) break;
		}
		if( tmp != NULL ) {
			c_puts( " ..." );
		}
		c_puts( "\n" );
	}

}
