/*
** Author:	Sol Boucher
**
** Description: Makes directories
*/
#include "userspace.h"

/*
** Create a directory at a user-specified path
*/

int main( void ) {
	c_puts( "mkdir running.\n" );
	
	writes( "Path to new directory? " );
	char dir_path[100];
	reads(100, dir_path);
	
	// Creates a directory with two sub directories
	if(mkdir(dir_path) == 1)
	{ // Failed to create file
		c_printf("Failed to create '%s'\n", dir_path);
		return 1;
	}
	
	c_puts( "mkdir exiting.\n" );
}
