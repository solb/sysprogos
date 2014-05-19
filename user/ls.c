/*
** Author:	Christopher Cooper
**
** Description: Performs a linux like ls operation to read contents of a directory
*/
#include "userspace.h"

#define SIZE	30

/*
** Prints the contents of a file out to SIO
*/

int main( void ) {
	c_puts( "ls running." );
	
	char *path = "";
	
	file_t entries[SIZE];
	
	int num_entries_read = readdir(path, entries, SIZE);
	
	if(num_entries_read)
	{//Failed to read directory
		c_puts("Reading directory failed.\n");
		return -1;
	}
	
	/*Prints all the entries that were read out in the following format
	**
	** If directory:	 d file_name
	** If not directory: - file_name
	*/
	for(int i = 0; i < num_entries_read; i++)
	{
		file_t file = entries[i];
	
		if(file.is_directory)
			writech('d');
		else
			writech('-');
		
		writech(' ');
		
		writes(file.name);
		
	}
	
	c_puts( "ls exiting\n" );
}