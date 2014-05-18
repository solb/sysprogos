/*
** File:	filesys.h
**
** Author:	Christopher Cooper (cac3479)
**
** Contributor:
**
** Description:	Filesystem-related declarations
*/

#ifndef _FILESYS_H_
#define _FILESYS_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

/*
** Defines the location at which the filesystem memory begins
*/
#define FS_MEM_LOC	 0;


/*
** Defines the value that represents the last cluster in a chain
*/
#define LAST_CLUSTER	0x0FFFFFF8

/*
** Defines the 2 "free" values for if a entry in a directory is freed
**
** 1. 0xE5 = Entry is free
** 2. 0x00 = Entry is free (same as 0xE5) and no allocated dir entries after this one
*/
#define ENTRY_FREE 		0xE5
#define ENTRIES_FREE	0x00

/*
** Defines the maximum number of files to be stored in a directory
*/
#define	MAX_DIRECTORY_SIZE	50;

/*
** Defines the file_entry ATTRIBUTES
*/
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

/*
** Start of C-only definitions
*/

#include "types.h"

/*
** Types
*/


// FAT32 Boot Sector
//
typedef struct fat32_bs {
	//Generic FAT
	ubyte_t		bootjmp[3];
	char		oem_name[8];
	ushort_t	bytes_per_sector;
	ubyte_t		sectors_per_cluster;
	ushort_t	reserved_sector_count;
	ubyte_t		num_fats;
	ushort_t	root_entry_count;
	ushort_t	total_sectors_16; //For FAT32, must be 0
	ubyte_t		media_type;
	ushort_t	table_size_16;	// For FAT32, must be 0
	ushort_t	sectors_per_track;
	ushort_t	heads_count;
	uint_t		hidden_sector_count;
	uint_t		total_sectors_32; //For FAT32, must be Non-zero
	
	//FAT 32 Specific
	uint_t		table_size_32;	//Count of sectors used for the FAT
	ushort_t	extended_flags;
	ushort_t	fat_version;
	uint_t		root_cluster;
	ushort_t	fat_info;
	ushort_t	backup_boot_sector;
	byte_t		reserved[12];
	ubyte_t		drive_number;
	ubyte_t		reserved_1;
	ubyte_t		boot_signature;
	uint_t		volume_id;
	byte_t		volume_label[11];
	byte_t		fat_type[8]; // For FAT32, "FAT32   "
	
} fat32_bs_t;		//sizeof = 96 bytes


//File structure
// (To make a directory: set ATTR_DIRECTORY bit in attributes
//						 set file size to 0)
//
typedef struct file_entry {
	char		name[12];
	ubyte_t		attributes;
	ubyte_t		reserved_NT; // Reserved for Windows NT. Set to 0
	ubyte_t		create_time_milli;
	ushort_t	create_time;
	ushort_t	create_date;
	ushort_t	last_access_date;
	ushort_t	first_cluster_hi;	//Relateive cluster number hi
	ushort_t	write_time;
	ushort_t	write_date;
	ushort_t	first_cluster_low;	//Relative cluster number lo
	uint_t		file_size;
} file_entry_t;		//sizeof = 36 bytes

/*
** Globals
*/

/*
** Pointer to the starting memory address of the filesystem
*/
byte_t *filesystem;

/*
** Structure containing the boot_sector information read from the filesystem
*/ 
fat32_bs_t boot_sector;

/*
** Contains the cluster size (in bytes)
*/
uint_t cluster_size;

/*
**	The sector number of the start of the data section
*/
uint_t	data_start_sector;

/*
** FAT start sector number
*/
uint_t fat_start_loc;


/*
** Prototypes
*/

/*
** _filesys_calc_absolute_cluster_loc - Calculates the absolute cluster location
**										using the relative cluster number
*/
uint_t _filesys_calc_absolute_cluster_loc(uint_t relative_cluster);


/*
** _filesys_find_next_cluster - Gets the next relative cluster number from FAT
**
*/
uint_t _filesys_find_next_cluster(uint_t current_cluster);

/*
** _filesys_calc_relative_cluster - Calculates the relative cluster number
**									using the exact memory address
*/
uint_t _filesys_calc_relative_cluster(uint_t cluster_address);

/*
** void _filesys_convert_to_shortname - Converts a given string to a valid Shortname
**										version to be used with the filesystem
*/
void _filesys_convert_to_shortname(char* filename, char* shortname);

/*
** char_toupper - Converts the passed in char to uppercase if it is an ascii character
*/
char char_toupper(char c);

/*
** find_first_free_entry - Finds the first free entry in the directory at the given
**							address and returns the memory location of it
**
**							Returns 0 if there were no free entries (needs to expand to
**								a new cluster)
*/
uint_t _filesys_find_first_free_entry(uint_t dir_address);

/*
** _filesys_expand_cluster_chain - Expands the cluster chain starting at the given start
**									relative cluster number and expands it to one more
**									cluster and returns the address to the newly added
**									cluster.
*/
uint_t _filesys_expand_cluster_chain(uint_t start_cluster);

/*
** _filesys_shrink_cluster_chain - Shrinks the cluster chain down to a single cluster
**									freeing up all the other clusters in the chain
** 
*/
void _filesys_shrink_cluster_chain(uint_t start_cluster);

/*
** _filesys_find_next_free_cluster - Goes through the FAT and locates the first available
**										cluster that is free and returns the relative
**										cluster number. Returns 0 if no free cluster found
*/
uint_t _filesys_find_next_free_cluster(void);

/*
** _filesys_write_file_entry - Writes a file entry at the given entry location using
**								the provided filename, attributes, and cluster number
*/
void _filesys_write_file_entry(uint_t new_entry_loc, char* filename, ubyte_t attributes, 
 								uint_t new_file_cluster);
 								
/*
** _filesys_update_fats - Updates the relative cluster entry in all of the FATS in the FS
**
*/
void _filesys_update_fats(uint_t relative_cluster, uint_t value);

/*
** _filesys_update_file_size - Updates the file's file size in it's entry by locating the 
**								entry in the filesystem using the given filename at the
**								parent directory location and updates it to the given
**								data length
*/
void _filesys_update_file_size(char *filename, file_entry_t *parent_dir, uint_t data_len);

/*
** _filesys_write_file - Writes the given data of the given length to the given file
**							If there is data in the file already, clears it
**
**							Returns SUCCESS if write is successful, FAILURE otherwise
*/
uint_t _filesys_write_file(char* path, byte_t *data, uint_t data_len);

/*
** _filesys_make_dir - Makes a new directory at the given path and sets the new_dir to 
**						the newly created file entry for the new directory.
**
**						Returns SUCCESS or FAILURE
*/
uint_t _filesys_make_dir(char* path, file_entry_t* new_dir);

/*
** _filesys_make_file - Creates a file at the given path location within
**						the filesystem using the given attributes and returns the file 
**						entry for the newly created file
**
*/
uint_t _filesys_make_file(char* path, ubyte_t attributes, file_entry_t* new_file);

/*
** _filesys_find_file - Given a file path and a folder address, it will find the file in 
**						the filesystem and copy the file_entry into the given file entry's
**						memory.
**
**						PATH Format: \folder1\folder2\<filename>
**
**						IF the folder_address is set to 0, it will default to using the
**						Root Directory
**
**  					 Return SUCCESS or FAILURE
**
*/
uint_t _filesys_find_file(char* path, file_entry_t* file, uint_t dir_address);

/*
** _filesys_readdir -  finds a directory at the given address and reads the given number
**						 of entries entries passed into the function, within the directory
**						 and stores each entry in the given file entry array.
**
**						Returns the number of entries that were added to the array
*/
uint_t _filesys_readdir(file_entry_t *entries, uint_t entries_size, uint_t dir_address);

/*
** _filesys_readfile - finds a file at the given file address and starts reading the
** 						file from the offset (file_address + offset) and it reads for
**						the given number of bytes.
**
**		The data gets read into the given buffer
**
*/
void _filesys_readfile(byte_t *buffer, uint_t file_address, uint_t offset, uint_t amount);


/*
** _filesys_init()
**
** initializes the FAT32 filesystem
*/
void _filesys_init( void );


#endif

#endif
