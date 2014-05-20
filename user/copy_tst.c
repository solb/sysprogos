/*
** Author:	Christopher Cooper
**
** Description: Tests various writing features of the filesystem and tests the associated
**					syscalls
*/
#include "userspace.h"

/*
** Prints the contents of a file out to SIO
*/

#define	SIZE	1000

char data[SIZE];

int main( void ) {
	c_puts( "copy_test running.\n" );
	
	//Creates a directory with two sub directories
	if(mkdir("/dir1") == 1)
	{//Failed to create file
		c_puts("Failed to create \"/dir1\"");
		return -1;
	}
		
	if(mkdir("/dir1/dir1-1") == 1)
	{//Failed to create file
		c_puts("Failed to create \"/dir1/dir1-1\"");
		return -1;
	}
		
	if(mkdir("/dir1/dir1-2") == 1)
	{//Failed to create file
		c_puts("Failed to create \"/dir1/dir1-2\"");
		return -1;
	}
		
	int num_bytes_read = readfile("/manpages/bogus.man", 0, data, SIZE);
	
	if(num_bytes_read < 0)
	{//An error occurred
		c_puts( "Unable to read file to copy!\n" );
		return -1;
	}
			
	if(writefile("/dir1/dir1-2/man_copy.txt", data, num_bytes_read) == 1)
	{//Error writing
		c_puts("Unable to write /dir1/dir1-1/man_copy.txt!\n");
		return -1;
	}
	
	c_puts( "copy_test exiting\n" );
}
