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
#define FS_SIZE_MB = 2;				//Size of the File system (in MB)
#define CLUSTER_SIZE = 4096; 		//Size of a cluster (in bytes)
#define SECTORS_PER_CLUSTER = 8;	//Number of sectors in a cluster

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
	ubyte_t		oem_name[8];
	ushort_t	bytes_per_sector;
	ubyte_t		sectors_per_cluster;
	ushort_t	reserved_sector_count;
	ubyte_t		table_count;
	ushort_t	root_entry_count;
	ushort_t	total_sectors_16; //For FAT32, must be 0
	ubyte_t		media_type;
	ushort_t	table_size_16;	// For FAT32, must be 0
	ushort_t	sectors_per_track;
	ushort_t	heads_count;
	uint_t		hidden_sector_count;
	uint_t		total_sectors_32; //For FAT32, must be Non-zero
	
	//FAT 32 Specific
	uint_t		table_size_32;
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
	ubyte_t		volume_lable[11];
	ubyte_t		fat_type[8]; // For FAT32, "FAT32   "
	
} fat32_bs_t;


//File structure
// (To make a directory: set ATTR_DIRECTORY bit in attributes
//						 set file size to 0)
//
typedef struct file {
	ubyte_t		name[11];
	ubyte_t		attributes;
	ubyte_t		reserved_NT; // Reserved for Windows NT. Set to 0
	ubyte_t		create_time_milli;
	ushort_t	create_time;
	ushort_t	create_date;
	ushort_t	last_acces_date;
	ushort_t	first_cluster_hi;
	ushort_t	write_time;
	ushort_t	write_date;
	ushort_t	first_cluster_low;
	uint_t		file_size;
} file_t;

/*
** Globals
*/


/*
** Prototypes
*/


#endif

#endif
