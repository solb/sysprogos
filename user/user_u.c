/*
** Author:	Sol Boucher
**
** Description: U userspace process
*/

#include "userspace.h"

/*
** User U prints strings.
*/

int main( void ) {
	int i;

	c_printf( "User U running\n" );
	i = writes( "Us1U" );
	c_printf( "User U string 1 %d chars\n", i );
	i = writes( "UHello, world!U" );
	c_printf( "User U string 2 %d chars\n", i );
	i = writes( "ULet's try a really long string for a changeU" );
	c_printf( "User U string 3 %d chars\n", i );

	c_printf( "User U exiting\n" );
	exit();
}
