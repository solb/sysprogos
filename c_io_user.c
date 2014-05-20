/*
** File:	c_io_user.c
**
** Author:	Dan Johnson
**
** Description:	Wrapper functions that are implemented via syscall in userspace
*/
#include "c_io.h"

void c_putchar( unsigned int c ) {
	c_putchar_at(-1, -1, c);
}

int c_getchar( void ){
	//TODO: implement this as a syscall
	return 0;
}
