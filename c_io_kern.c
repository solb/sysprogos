/*
** SCCS ID:	@(#)c_io.c	1.14	4/8/14
**
** File:	c_io.c
**
** Author:	K. Reek
**
** Contributor:	Jon Coles, Warren R. Carithers
**
** Description:	Console I/O routines
**
**	This module implements a simple set of input and output routines
**	for the console screen and keyboard on the Soyatas in the DSL.
**	Refer to the printed documentation for complete details.
**
*/

#include "c_io.h"
#include "startup.h"
#include "support.h"
#include "x86arch.h"
#include <stdarg.h>

/*
** Video parameters, and state variables
*/
#define	SCREEN_MIN_X	0
#define	SCREEN_MIN_Y	0
#define	SCREEN_X_SIZE	80
#define	SCREEN_Y_SIZE	25
#define	SCREEN_MAX_X	( SCREEN_X_SIZE - 1 )
#define	SCREEN_MAX_Y	( SCREEN_Y_SIZE - 1 )

extern unsigned int	scroll_min_x, scroll_min_y;
extern unsigned int	scroll_max_x, scroll_max_y;
unsigned int	curr_x, curr_y;
extern unsigned int	min_x, min_y;
extern unsigned int	max_x, max_y;

#define	VIDEO_ADDR(x,y)	( unsigned short * ) \
		( VIDEO_BASE_ADDR + 2 * ( (unsigned long long)(y) * SCREEN_X_SIZE + (unsigned long long)(x) ) )

/*
** Support routines.
**
** bound: confine an argument within given bounds
** __c_putchar_at: physical output to the video memory
** __c_setcursor: set the cursor location (screen coordinates)
*/
static unsigned int bound( unsigned int min, unsigned int value, unsigned int max ){
	if( value < min ){
		value = min;
	}
	if( value > max ){
		value = max;
	}
	return value;
}

static void __c_setcursor( void ){
	unsigned addr;
	unsigned int	y = curr_y;

	if( y > scroll_max_y ){
		y = scroll_max_y;
	}

	addr = (unsigned)( y * SCREEN_X_SIZE + curr_x );

	__outb( 0x3d4, 0xe );
	__outb( 0x3d5, ( addr >> 8 ) & 0xff );
	__outb( 0x3d4, 0xf );
	__outb( 0x3d5, addr & 0xff );
}

static void __c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
	/*
	** If x or y is too big or small, don't do any output.
	*/
	if( x <= max_x && y <= max_y ){
		unsigned short *addr = VIDEO_ADDR( x, y );

		if( c > 0xff ) {
			/*
			** Use the given attributes
			*/
			*addr = (unsigned short)c;
		} else {
			/*
			** Use attributes 0000 0111 (white on black)
			*/
			*addr = (unsigned short)c | 0x0700;
		}
	}
}

void c_setscroll( unsigned int s_min_x, unsigned int s_min_y, unsigned int s_max_x, unsigned int s_max_y ){
	scroll_min_x = bound( min_x, s_min_x, max_x );
	scroll_min_y = bound( min_y, s_min_y, max_y );
	scroll_max_x = bound( scroll_min_x, s_max_x, max_x );
	scroll_max_y = bound( scroll_min_y, s_max_y, max_y );
	curr_x = scroll_min_x;
	curr_y = scroll_min_y;
	__c_setcursor();
}

/*
** Cursor movement in the scroll region
*/
void c_moveto( unsigned int x, unsigned int y ){
	curr_x = bound( scroll_min_x, x + scroll_min_x, scroll_max_x );
	curr_y = bound( scroll_min_y, y + scroll_min_y, scroll_max_y );
	__c_setcursor();
}

/*
** The putchar family
*/
void c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
	if( ( c & 0x7f ) == '\n' ){
		unsigned int	limit;

		/*
		** If we're in the scroll region, don't let this loop
		** leave it.  If we're not in the scroll region, don't
		** let this loop enter it.
		*/
		if( x > scroll_max_x ){
			limit = max_x;
		}
		else if( x >= scroll_min_x ){
			limit = scroll_max_x;
		}
		else {
			limit = scroll_min_x - 1;
		}
		while( x <= limit ){
			__c_putchar_at( x, y, ' ' );
			x += 1;
		}
	}
	else {
		__c_putchar_at( x, y, c );
	}
}

#ifndef SA_DEBUG
void c_putchar( unsigned int c ){
	/*
	** If we're off the bottom of the screen, scroll the window.
	*/
	if( curr_y > scroll_max_y ){
		c_scroll( curr_y - scroll_max_y );
		curr_y = scroll_max_y;
	}

	switch( c & 0xff ){
	case '\n':
		/*
		** Erase to the end of the line, then move to new line
		** (actual scroll is delayed until next output appears).
		*/
		while( curr_x <= scroll_max_x ){
			__c_putchar_at( curr_x, curr_y, ' ' );
			curr_x += 1;
		}
		curr_x = scroll_min_x;
		curr_y += 1;
		break;

	case '\r':
		curr_x = scroll_min_x;
		break;

	default:
		__c_putchar_at( curr_x, curr_y, c );
		curr_x += 1;
		if( curr_x > scroll_max_x ){
			curr_x = scroll_min_x;
			curr_y += 1;
		}
		break;
	}
	__c_setcursor();
}
#endif

void c_clearscroll( void ){
	unsigned int	nchars = scroll_max_x - scroll_min_x + 1;
	unsigned int	l;
	unsigned int	c;

	for( l = scroll_min_y; l <= scroll_max_y; l += 1 ){
		unsigned short *to = VIDEO_ADDR( scroll_min_x, l );

		for( c = 0; c < nchars; c += 1 ){
			*to++ = ' ' | 0x0700;
		}
	}
}

void c_clearscreen( void ){
	unsigned short *to = VIDEO_ADDR( min_x, min_y );
	unsigned int	nchars = ( max_y - min_y + 1 ) * ( max_x - min_x + 1 );

	while( nchars > 0 ){
		*to++ = ' ' | 0x0700;
		nchars -= 1;
	}
}


void c_scroll( unsigned int lines ){
	unsigned short *from;
	unsigned short *to;
	int	nchars = scroll_max_x - scroll_min_x + 1;
	int	line, c;

	/*
	** If # of lines is the whole scrolling region or more, just clear.
	*/
	if( lines > scroll_max_y - scroll_min_y ){
		c_clearscroll();
		curr_x = scroll_min_x;
		curr_y = scroll_min_y;
		__c_setcursor();
		return;
	}

	/*
	** Must copy it line by line.
	*/
	for( line = scroll_min_y; line <= scroll_max_y - lines; line += 1 ){
		from = VIDEO_ADDR( scroll_min_x, line + lines );
		to = VIDEO_ADDR( scroll_min_x, line );
		for( c = 0; c < nchars; c += 1 ){
			*to++ = *from++;
		}
	}

	for( ; line <= scroll_max_y; line += 1 ){
		to = VIDEO_ADDR( scroll_min_x, line );
		for( c = 0; c < nchars; c += 1 ){
			*to++ = ' ' | 0x0700;
		}
	}
}

unsigned char scan_code[ 2 ][ 128 ] = {
	{
/* 00-07 */	'\377',	'\033',	'1',	'2',	'3',	'4',	'5',	'6',
/* 08-0f */	'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
/* 10-17 */	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
/* 18-1f */	'o',	'p',	'[',	']',	'\n',	'\377',	'a',	's',
/* 20-27 */	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',
/* 28-2f */	'\'',	'`',	'\377',	'\\',	'z',	'x',	'c',	'v',
/* 30-37 */	'b',	'n',	'm',	',',	'.',	'/',	'\377',	'*',
/* 38-3f */	'\377',	' ',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 40-47 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'7',
/* 48-4f */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50-57 */	'2',	'3',	'0',	'.',	'\377',	'\377',	'\377',	'\377',
/* 58-5f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 60-67 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 68-6f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 70-77 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 78-7f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377'
	},

	{
/* 00-07 */	'\377',	'\033',	'!',	'@',	'#',	'$',	'%',	'^',
/* 08-0f */	'&',	'*',	'(',	')',	'_',	'+',	'\b',	'\t',
/* 10-17 */	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
/* 18-1f */	'O',	'P',	'{',	'}',	'\n',	'\377',	'A',	'S',
/* 20-27 */	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
/* 28-2f */	'"',	'~',	'\377',	'|',	'Z',	'X',	'C',	'V',
/* 30-37 */	'B',	'N',	'M',	'<',	'>',	'?',	'\377',	'*',
/* 38-3f */	'\377',	' ',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 40-47 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'7',
/* 48-4f */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50-57 */	'2',	'3',	'0',	'.',	'\377',	'\377',	'\377',	'\377',
/* 58-5f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 60-67 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 68-6f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 70-77 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 78-7f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377'
	}
};

#define	C_BUFSIZE	200
#define	KEYBOARD_DATA	0x60
#define	KEYBOARD_STATUS	0x64
#define	READY		0x1
#define	EOT		'\04'

/*
** Circular buffer for input characters.  Characters are inserted at
** __c_next_space, and are removed at __c_next_char.  Buffer is empty if
** these are equal.
*/
static	char	__c_input_buffer[ C_BUFSIZE ];
static	volatile char	*__c_next_char = __c_input_buffer;
static	volatile char	*__c_next_space = __c_input_buffer;

static	volatile char *__c_increment( volatile char *pointer ){
	if( ++pointer >= __c_input_buffer + C_BUFSIZE ){
		pointer = __c_input_buffer;
	}
	return pointer;
}

static void __c_input_scan_code( int code ){
	static	int	shift = 0;
	static	int	ctrl_mask = 0xff;

	/*
	** Do the shift processing
	*/
	code &= 0xff;
	switch( code ){
	case 0x2a:
	case 0x36:
		shift = 1;
		break;

	case 0xaa:
	case 0xb6:
		shift = 0;
		break;

	case 0x1d:
		ctrl_mask = 0x1f;
		break;

	case 0x9d:
		ctrl_mask = 0xff;
		break;

	default:
		/*
		** Process ordinary characters only on the press
		** (to handle autorepeat).
		** Ignore undefined scan codes.
		*/
		if( ( code & 0x80 ) == 0 ){
			code = scan_code[ shift ][ (int)code ];
			if( code != '\377' ){
				volatile char	*next = __c_increment( __c_next_space );

				/*
				** Store character only if there's room
				*/
				if( next != __c_next_char ){
					*__c_next_space = code & ctrl_mask;
					__c_next_space = next;
				}
			}
		}
	}
}

static void __c_keyboard_isr( int vector, int code ){
	__c_input_scan_code( __inb( KEYBOARD_DATA ) );
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
}

int c_getchar( void ){
	char	c;
	int	interrupts_enabled = __get_flags() & EFLAGS_IF;

	while( __c_next_char == __c_next_space ){
		if( !interrupts_enabled ){
			/*
			** Must read the next keystroke ourselves.
			*/
			while( ( __inb( KEYBOARD_STATUS ) & READY ) == 0 ){
				;
			}
			__c_input_scan_code( __inb( KEYBOARD_DATA ) );
		}
	}

	c = *__c_next_char & 0xff;
	__c_next_char = __c_increment( __c_next_char );
	if( c != EOT ){
		c_putchar( c );
	}
	return c;
}

int c_input_queue( void ){
	int	n_chars = __c_next_space - __c_next_char;

	if( n_chars < 0 ){
		n_chars += C_BUFSIZE;
	}
	return n_chars;
}

/*
** Initialization routines
*/
void c_io_init( void ){
	/*
	** Screen dimensions
	*/
	min_x  = SCREEN_MIN_X;
	min_y  = SCREEN_MIN_Y;
	max_x  = SCREEN_MAX_X;
	max_y  = SCREEN_MAX_Y;

	/*
	** Scrolling region
	*/
	scroll_min_x = SCREEN_MIN_X;
	scroll_min_y = SCREEN_MIN_Y;
	scroll_max_x = SCREEN_MAX_X;
	scroll_max_y = SCREEN_MAX_Y;

	/*
	** Initial cursor location
	*/
	curr_y = min_y;
	curr_x = min_x;
	__c_setcursor();

	/*
	** Set up the interrupt handler for the keyboard
	*/
	__install_isr( INT_VEC_KEYBOARD, __c_keyboard_isr );

}
