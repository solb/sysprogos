/*
** Author:	Christopher Cooper
**
** Description: A program that will allow you to delete a file or directory
*/
#include "userspace.h"

/*
** Deletes a file at a given path
*/
int main( void ) {
	c_puts( "delete running.\n" );
	
	writes( "Path to file? " );
	char *path[100];
	reads(100, path);

	delete(path);
	
	c_puts( "delete exiting\n" );
}