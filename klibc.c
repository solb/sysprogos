/*
** SCCS ID:	%W%	%G%
**
** File:	klibc.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	C implementations of kernel library functions
*/

#define	__SP_KERNEL__

#include "common.h"

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

// status value strings
//
// (must track status_t values in defs.h)

const char *_status_strings[] = {
	"success",		// SUCCESS
	"failure",		// FAILURE
	"bad param",		// BAD_PARAM
	"empty queue",		// EMPTY_QUEUE
	"not empty queue",	// NOT_EMPTY_QUEUE
	"not found",		// NOT_FOUND
	"allocation failed",	// ALLOC_FAILED
};

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/

void _put_char_or_code( int ch ) {

	if( ch >= ' ' && ch < 0x7f ) {
		c_putchar( ch );
	} else {
		c_printf( "\\x%02x", ch );
	}
}

/*
** _kmemclr - initialize all bytes of a block of memory to zero
**
** usage:  _kmemclr( buffer, length )
*/

void _kmemclr( register byte_t *buf, register uint32_t len ) {

	while( len-- ) {
		*buf++ = 0;
	}

}

/*
** _kmemcpy - copy a block from one place to another
**
** usage:  _kmemcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _kmemcpy( register byte_t *dst, register byte_t *src, register uint32_t len ) {

	while( len-- ) {
		*dst++ = *src++;
	}

}


/*
** _kpanic - kernel-level panic routine
**
** usage:  _kpanic( module, msg, code )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
**
** 'module' argument is always printed; 'msg' argument is printed
** if it isn't NULL, followed by a newline
**
** 'msg' argument should contain a %s format code if the 'code'
** argument is to be printed as part of the panic message
**
** if 'code' is invalid, it will also be printed in integer form
*/

void _kpanic( char *module, char *msg, status_t code ) {

	c_puts( "\n\n***** KERNEL PANIC *****\n\n" );
	c_printf( "Module: %s\n", module );
	if( msg != NULL ) {
		c_printf( msg, _kstatus(code) );
		c_putchar( '\n' );
	}
	if( code >= N_STATUS ) {
		c_printf( "*** bad code %d\n", code );
	}

	//
	// This might be a good place to do a stack frame
	// traceback
	//

	__panic( "KERNEL PANIC" );

}
