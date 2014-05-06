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
** Defines Filesystem constants
*/
#define FS_SIZE_MB			2		//Size of the File system (in MB)
#define CLUSTER_SIZE		4096 	//Size of a cluster (in bytes)
#define SECTORS_PER_CLUSTER	8		//Number of sectors in a cluster


/*
** Defines the location at which the filesystem memory begins
*/
#define FS_MEM_LOC	 0;


/*
** Defines the value that represents the last cluster in a chain
*/
#define LAST_CLUSTER	0x0FFFFFF8

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
	ubyte_t		reserved[12];
	ubyte_t		drive_number;
	ubyte_t		reserved_1;
	ubyte_t		boot_signature;
	uint_t		volume_id;
	ubyte_t		volume_label[11];
	ubyte_t		fat_type[8]; // For FAT32, "FAT32   "
	
} fat32_bs_t;		//sizeof = 96 bytes


//File structure
// (To make a directory: set ATTR_DIRECTORY bit in attributes
//						 set file size to 0)
//
typedef struct file_entry {
	ubyte_t		name[11];
	ubyte_t		attributes;
	ubyte_t		reserved_NT; // Reserved for Windows NT. Set to 0
	ubyte_t		create_time_milli;
	ushort_t	create_time;
	ushort_t	create_date;
	ushort_t	last_acces_date;
	ushort_t	first_cluster_hi;	//Relateive cluster number hi
	ushort_t	write_time;
	ushort_t	write_date;
	ushort_t	first_cluster_low;	//Relative cluster number lo
	uint_t		file_size;
} file_entry_t;		//sizeof = 32 bytes

/*
** Globals
*/

/*
** Pointer to the starting memory address of the filesystem
*/
ubyte_t *filesystem;

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
** _filesys_readfile - finds a file at the given file address and starts reading the
** 						file from the offset (file_address + offset) and it reads for
**						the given number of bytes.
**
**		The data gets read into the given buffer
**
*/
void _filesys_readfile(ubyte_t *buffer, uint_t file_address, uint_t offset, uint_t amount);


/*
** _filesys_init()
**
** initializes the FAT32 filesystem
*/
void _filesys_init( void );


#endif

#endif
