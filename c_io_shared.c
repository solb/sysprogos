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

unsigned int	scroll_min_x = SCREEN_MIN_X;
unsigned int	scroll_min_y = SCREEN_MIN_Y;
unsigned int	scroll_max_x = SCREEN_X_SIZE;
unsigned int	scroll_max_y = SCREEN_Y_SIZE;
unsigned int	min_x = SCREEN_MIN_X;
unsigned int	min_y = SCREEN_MIN_Y;
unsigned int	max_x = SCREEN_X_SIZE;
unsigned int	max_y = SCREEN_Y_SIZE;

static unsigned int __c_strlen( char const *str ){
	unsigned int	len = 0;

	while( *str++ != '\0' ){
		len += 1;
	}
	return len;
}

/*
** The puts family
*/
void c_puts_at( unsigned int x, unsigned int y, char *str ){
	unsigned int	ch;

	while( (ch = *str++) != '\0' && x <= max_x ){
		c_putchar_at( x, y, ch );
		x += 1;
	}
}

#ifndef SA_DEBUG
void c_puts( char *str ){
	unsigned int	ch;

	while( (ch = *str++) != '\0' ){
		c_putchar( ch );
	}
}
#endif

char * cvtdec0( char *buf, int value ){
	int	quotient;

	quotient = value / 10;
	if( quotient < 0 ){
		quotient = 214748364;
		value = 8;
	}
	if( quotient != 0 ){
		buf = cvtdec0( buf, quotient );
	}
	*buf++ = value % 10 + '0';
	return buf;
}

int cvtdec( char *buf, int value ){
	char	*bp = buf;

	if( value < 0 ){
		*bp++ = '-';
		value = -value;
	}
	bp = cvtdec0( bp, value );
	*bp = '\0';

	return bp - buf;
}

char hexdigits[] = "0123456789ABCDEF";

int cvthex( char *buf, int value ){
	int	i;
	int	chars_stored = 0;
	char	*bp = buf;

	for( i = 0; i < 8; i += 1 ){
		int	val;

		val = ( value & 0xf0000000 );
		if( i == 7 || val != 0 || chars_stored ){
			chars_stored = 1;
			val >>= 28;
			val &= 0xf;
			*bp++ = hexdigits[ val ];
		}
		value <<= 4;
	}
	*bp = '\0';

	return bp - buf;
}

int cvtoct( char *buf, int value ){
	int	i;
	int	chars_stored = 0;
	char	*bp = buf;
	int	val;

	val = ( value & 0xc0000000 );
	val >>= 30;
	for( i = 0; i < 11; i += 1 ){

		if( i == 10 || val != 0 || chars_stored ){
			chars_stored = 1;
			val &= 0x7;
			*bp++ = hexdigits[ val ];
		}
		value <<= 3;
		val = ( value & 0xe0000000 );
		val >>= 29;
	}
	*bp = '\0';

	return bp - buf;
}

static int pad( int x, int y, int extra, int padchar ){
	while( extra > 0 ){
		if( x != -1 || y != -1 ){
			c_putchar_at( x, y, padchar );
			x += 1;
		}
		else {
			c_putchar( padchar );
		}
		extra -= 1;
	}
	return x;
}

static int padstr( int x, int y, char *str, int len, int width, int leftadjust, int padchar ){
	int	extra;

	if( len < 0 ){
		len = __c_strlen( str );
	}
	extra = width - len;
	if( extra > 0 && !leftadjust ){
		x = pad( x, y, extra, padchar );
	}
	if( x != -1 || y != -1 ){
		c_puts_at( x, y, str );
		x += len;
	}
	else {
		c_puts( str );
	}
	if( extra > 0 && leftadjust ){
		x = pad( x, y, extra, padchar );
	}
	return x;
}

static void __c_do_printf( int x, int y, char *fmt, va_list args ){
	char	buf[ 12 ];
	char	ch;
	char	*str;
	int	leftadjust;
	int	width;
	int	len;
	int	padchar;

	/*
	** Get characters from the format string and process them
	*/
	while( (ch = *fmt++) != '\0' ){
		/*
		** Is it the start of a format code?
		*/
		if( ch == '%' ){
			/*
			** Yes, get the padding and width options (if there).
			** Alignment must come at the beginning, then fill,
			** then width.
			*/
			leftadjust = 0;
			padchar = ' ';
			width = 0;
			ch = *fmt++;
			if( ch == '-' ){
				leftadjust = 1;
				ch = *fmt++;
			}
			if( ch == '0' ){
				padchar = '0';
				ch = *fmt++;
			}
			while( ch >= '0' && ch <= '9' ){
				width *= 10;
				width += ch - '0';
				ch = *fmt++;
			}

			/*
			** What data type do we have?
			*/
			switch( ch ){
			case 'c':
				ch = va_arg(args, int);
				buf[ 0 ] = ch;
				buf[ 1 ] = '\0';
				x = padstr( x, y, buf, 1, width, leftadjust, padchar );
				break;

			case 'd':
				len = cvtdec( buf, va_arg(args, int) );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 's':
				str = va_arg(args, char*);
				x = padstr( x, y, str, -1, width, leftadjust, padchar );
				break;

			case 'x':
				len = cvthex( buf, va_arg(args, int) );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 'o':
				len = cvtoct( buf, va_arg(args, int) );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			}
		}
		else {
			if( x != -1 || y != -1 ){
				c_putchar_at( x, y, ch );
				switch( ch ){
				case '\n':
					y += 1;
					/* FALL THRU */

				case '\r':
					x = scroll_min_x;
					break;

				default:
					x += 1;
				}
			}
			else {
				c_putchar( ch );
			}
		}
	}
}

void c_printf_at( unsigned int x, unsigned int y, char *fmt, ... ){
	va_list v;
	va_start(v, fmt);
	__c_do_printf( x, y, fmt, v );
	va_end(v);
}

void c_printf( char *fmt, ... ){
	va_list v;
	va_start(v, fmt);
	__c_do_printf( -1, -1, fmt, v );
	va_end(v);
}

#define	EOT		'\04'

int c_gets( char *buffer, unsigned int size ){
	char	ch;
	int	count = 0;

	while( size > 1 ){
		ch = c_getchar();
		if( ch == EOT ){
			break;
		}
		*buffer++ = ch;
		count += 1;
		size -= 1;
		if( ch == '\n' ){
			break;
		}
	}
	*buffer = '\0';
	return count;
}
