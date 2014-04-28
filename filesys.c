/*
** File:	filesys.c
**
** Author:	Christopher Cooper (cac3479)
**
** Contributor: 
**
** Description:	Filesystem-related implementations
*/

#define	__SP_KERNEL__

#include "common.h"

#include "filesys.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/



/*
** PUBLIC GLOBAL VARIABLES
*/



/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _filesys_init()
**
** initializes the FAT32 filesystem
*/
void _filesys_init( void )
{
	filesystem = FS_MEM_LOC;
	
	//ZEROS out all filesystem memory
	uint_t size = FS_SIZE_MB * 1024 * 1024; //Converts size from MB to B
	while(size--)
	{
		*filesystem++ = (ubyte_t) 0;
	}
	
	//configure fat32_bs
	
	
	//generate FAT with root directory
}
