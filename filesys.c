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
** _filesys_calc_absolute_cluster_loc - Calculates the absolute cluster location
**										using the relative cluster number
*/
uint_t _filesys_calc_absolute_cluster_loc(uint_t relative_cluster)
{
	uint_t absolute_cluster = relative_cluster - 2 + (data_start_sector / 
												boot_sector.sectors_per_cluster);
	
	return cluster_size * absolute_cluster;											
}


/*
** _filesys_find_next_cluster - Gets the next relative cluster number from FAT
**
*/
uint_t _filesys_find_next_cluster(uint_t current_cluster)
{
	return  *(uint_t *)&filesystem[fat_start_loc + current_cluster * 4];
}

/*
** _filesys_calc_relative_cluster - Calculates the relative cluster number
**									using the exact memory address
*/
uint_t _filesys_calc_relative_cluster(uint_t cluster_address)
{
	uint_t absolute_cluster = cluster_address / (boot_sector.bytes_per_sector * 
													boot_sector.sectors_per_cluster);
													
	return absolute_cluster + 2 - (data_start_sector / boot_sector.sectors_per_cluster);
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _filesys_readdir -  finds a directory at the given address and reads all the file
**						entries within the directory and stores each entry in the given
**						file entry array.
**						
**						Returns the number of entries
*/
uint_t _filesys_readdir(file_entry_t *entries, uint_t dir_address)
{
	//Currently assuming entries is large enough to hold all entries.
	//This needs to be fixed since the number of entries in a directory is an unknown
	
	//Cheats by using _filesys_readfile to read the byte data of the directory entries
	//which can then be parsed without dealing with cluster chains. Again, currently
	//will be assuming a directory is no longer than 1 cluster in size.
	byte_t directory_data[cluster_size];
	_filesys_readfile(directory_data, dir_address, 0, cluster_size);
	
	uint_t entry_count = 0;
	uint_t data_offset = 0;
	while(directory_data[data_offset] != ENTRIES_FREE || data_offset == cluster_size)
	{
		//Parses a file entry
		file_entry_t file = 
		{
			.name = {0},
			.attributes = directory_data[data_offset+11],
			.reserved_NT = directory_data[data_offset+12],
			.create_time_milli = directory_data[data_offset+13],
			.create_time = *(ushort_t*)&directory_data[data_offset+14],
			.create_date = *(ushort_t*)&directory_data[data_offset+16],
			.last_access_date = *(ushort_t*)&directory_data[data_offset+18],
			.first_cluster_hi = *(ushort_t*)&directory_data[data_offset+20],
			.write_time = *(ushort_t*)&directory_data[data_offset+22],
			.write_date = *(ushort_t*)&directory_data[data_offset+24],
			.first_cluster_low = *(ushort_t*)&directory_data[data_offset+26],
			.file_size = *(uint_t*)&directory_data[data_offset+28]
		};
		
		_kmemcpy(file.name, directory_data+data_offset, 11);
		
		//File entry valid, add to entries array
		if(file.name[0] != ENTRY_FREE && file.name[0] != ENTRIES_FREE)
		{
			entries[entry_count] = file;
			entry_count++;
		}
		
		data_offset += 32; //Increments offset to next entries
	}
	
	return entry_count;
}

/*
** _filesys_readfile - finds a file at the given file address and starts reading the
** 						file from the offset (file_address + offset) and it reads for
**						the given number of bytes (count) and returns a byte array of 
**						the data that was read.
**
*/
void _filesys_readfile(byte_t *buffer, uint_t file_address, uint_t offset, uint_t count)
{
	uint_t relative_cluster = _filesys_calc_relative_cluster(file_address);
	uint_t next_cluster = _filesys_find_next_cluster(relative_cluster);
	
	uint_t filesys_offset = file_address + offset;

	uint_t remaining_bytes = count;
	uint_t remaining_bytes_current_cluster = cluster_size - offset;
	while(remaining_bytes > 0)
	{
		uint_t buffer_position = count - remaining_bytes;
	
		if(remaining_bytes <= remaining_bytes_current_cluster)
		{ //Last set of bytes to read
			_kmemcpy(buffer+buffer_position, filesystem+filesys_offset, remaining_bytes);
			remaining_bytes = 0;
			
			continue;
		}
		else if(next_cluster >= LAST_CLUSTER )
		{ //Last cluster in file cluster chain
			/*This prevents the issue if the count is > file size. If this occurs
			*then it will reach the last cluster in the chain and just copy the 
			*entire cluster size
			*/
			_kmemcpy(buffer+buffer_position, filesystem+filesys_offset, cluster_size);
			break;
		}
		
		//Spans at least another cluster
		_kmemcpy(buffer+buffer_position, filesystem+filesys_offset, cluster_size);
		
		//Updates remaining_bytes
		remaining_bytes -= cluster_size;
		
		//Updates the file_address to next cluster location
		filesys_offset = _filesys_calc_absolute_cluster_loc(next_cluster);
		next_cluster = _filesys_find_next_cluster(next_cluster);
	}
}

/*
** _filesys_init()
**
** initializes the FAT32 filesystem
*/
void _filesys_init( void )
{
	//Loads the filesystem into memory
	filesystem = FS_MEM_LOC;	
	
	//Read in filesystem boot sector information
	boot_sector = (fat32_bs_t)
		{ 
			.bootjmp = {filesystem[0], filesystem[1], filesystem[2]}, 	//3 bytes
			.oem_name = {0},											//8 bytes
			.bytes_per_sector = *(ushort_t*)&filesystem[11], 			//2 bytes
			.sectors_per_cluster = filesystem[13], 						//1 byte
			.reserved_sector_count = *(ushort_t*)&filesystem[14], 		//2 bytes
			.num_fats = filesystem[16], 								//1 byte
			.root_entry_count = *(ushort_t*)&filesystem[17], 			//2 bytes
			.total_sectors_16 = *(ushort_t*)&filesystem[19], 			//2 bytes
			.media_type = filesystem[21], 								//1 byte
			.table_size_16 = *(ushort_t*)&filesystem[22], 				//2 bytes
			.sectors_per_track = *(ushort_t*)&filesystem[24], 			//2 bytes
			.heads_count = *(ushort_t*)&filesystem[26], 				//2 bytes
			.hidden_sector_count = *(uint_t*)&filesystem[28], 			//4 bytes
			.total_sectors_32 = *(uint_t*)&filesystem[32], 				//4 bytes
			.table_size_32 = *(uint_t*)&filesystem[36], 				//4 bytes
			.extended_flags = *(ushort_t*)&filesystem[40], 				//2 bytes
			.fat_version = *(ushort_t*)&filesystem[42], 				//2 bytes
			.root_cluster = *(uint_t*)&filesystem[44], 					//4 bytes
			.fat_info = *(ushort_t*)&filesystem[48], 					//2 bytes
			.backup_boot_sector = *(ushort_t*)&filesystem[50], 			//2 bytes
			.reserved = {0},				 							//12 bytes
			.drive_number = filesystem[64], 							//1 byte
			.reserved_1 = filesystem[65], 								//1 byte
			.boot_signature = filesystem[66], 							//1 byte
			.volume_id = *(uint_t*)&filesystem[67], 					//4 bytes
			.volume_label = {0}, 										//11 bytes
			.fat_type = {0} 											//8 bytes
		};
	
	//Copies the arrays over to the boot_sector structure
	//NEEDS TO BE CHANGED TO _kmemcpy WHEN PORTED TO OS
	_kmemcpy(boot_sector.oem_name, filesystem+3, 8);
	_kmemcpy(boot_sector.reserved, filesystem+52, 12);
	_kmemcpy(boot_sector.volume_label, filesystem+71, 11);
	_kmemcpy(boot_sector.fat_type, filesystem+82, 8);
	
	cluster_size = boot_sector.bytes_per_sector * boot_sector.sectors_per_cluster;
	data_start_sector = boot_sector.reserved_sector_count + 
									(boot_sector.num_fats * boot_sector.table_size_32);
	fat_start_loc = boot_sector.reserved_sector_count * boot_sector.bytes_per_sector;
}
