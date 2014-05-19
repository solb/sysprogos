/*
** Author:	Christopher Cooper
**
** Description: Performs a linux like cat operation on a file
*/

#include "userspace.h"

#define SIZE	512

/*
** Prints the contents of a file out to SIO
*/

int main( void ) {
	c_puts( "cat running." );
	
	int num_bytes_read = 0;
	int offset = 0;
	
	char *file_path = "";
	
	//Reads the file in increments of amount SIZE. Continues until it reaches end of file
	// (which is when readfile returns 0)
	do
	{
		byte_t buffer[SIZE] = { 0 };
		num_bytes_read = readfile(file_path, offset, buffer, SIZE); //readfile SYSCALL

		if(num_bytes_read < 0)
		{//An error occurred
			c_puts( "Failed to finish reading file for cat\n" );
		}
		
		//Writes the data that has been read to SIO
		for(int i = 0; i < num_bytes_read; i++)
		{
			writech(buffer[i]);
		}
		
		offset += num_bytes_read;

	} while(num_bytes_read > 0);
	
	c_puts( "cat exiting\n" );
}