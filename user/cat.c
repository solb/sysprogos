/*
** Author:	Christopher Cooper
**
** Description: Performs a linux like cat operation on a file
*/

#include "userspace.h"

#define SIZE	512
#define MAX_LINES 24

/*
** Prints the contents of a file out to SIO
*/

int main( void ) {
	c_puts( "cat running.\n" );
	
	int num_bytes_read = 0;
	int offset = 0;
	
	writes( "Path to file? " );
	char file_path[100];
	reads(100, file_path);
	
	int lines_read = 0;

	//Reads the file in increments of amount SIZE. Continues until it reaches end of file
	// (which is when readfile returns 0)
	do
	{
		byte_t buffer[SIZE] = { 0 };
		num_bytes_read = readfile(file_path, offset, buffer, SIZE); //readfile SYSCALL

		if(num_bytes_read < 0)
		{//An error occurred
			writes( "Unable to read file!\n" );
			return -1;
		}
		
		//Writes the data that has been read to SIO
		for(int i = 0; i < num_bytes_read; i++)
		{
			char ch = buffer[i];
			if (ch == '\n') {
				lines_read++;
				while (lines_read == MAX_LINES) {
					switch(readch()) {
					case '\n':
						lines_read--;
						break;
					case ' ':
						lines_read = 0;
						break;
					case 'q':
						writech('\n');
						exit();
					}
				}
			}
			writech(buffer[i]);
		}
		
		offset += num_bytes_read;

	} while(num_bytes_read > 0);
	
	c_puts( "cat exiting.\n" );
}
