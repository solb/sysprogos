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
	return  *(uint_t *)&filesystem[fat_start_loc + current_cluster * 4] & 0x0FFFFFFF;
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
** void _filesys_convert_to_shortname - Converts a given string to a valid Shortname
**										version to be used with the filesystem
*/
void _filesys_convert_to_shortname(char* filename, char* shortname)
{	
	uint_t index = 0;
	while(index < 8)
	{//Filename portion of the shortname
		if(*filename == '/' || *filename == '.' || *filename == '\0')
		{//Reached end of filename or found a '.' before getting 8 characters 
			shortname[index] = ' ';
		}
		else
		{
			shortname[index] = char_toupper(*filename);
			filename++;
		}
		index++;
	}
	
	while(*filename != '.' && *filename != '\0')
	{//Increments the filename until it either finds a '.' or reaches the end
		filename++;
	}
	
	if(*filename == '.') filename++; //Moves to next char after the '.'
	
	while(index < 11)
	{//Extension portion of shortname
		if(*filename == '\0')
		{
			shortname[index] = ' ';
		}
		else
		{
			shortname[index] = char_toupper(*filename);
			filename++;
		}
		index++;
	}
	
	shortname[11] = '\0';	
}

/*
** char_toupper - Converts the passed in char to uppercase if it is an ascii character
*/
char char_toupper(char c)
{
	if(c >= 'a' && c <= 'z')
	{
		int upper_dist = 'A' - 'a';
		return c + upper_dist;
	}
	else
	{//Not in range of lowercase characters
		return c;
	}
}

/*
** find_first_free_entry - Finds the first free entry in the directory at the given
**							address and returns the memory location of it
**
**							Returns 0 if there were no free entries (needs to expand to
**								a new cluster)
*/
uint_t _filesys_find_first_free_entry(uint_t dir_address)
{
	uint_t entry_address = dir_address;
	uint_t end_of_cluster = dir_address + cluster_size;
	while(entry_address != end_of_cluster)
	{//loops until it reaches the end of the current cluster
		ubyte_t val = filesystem[entry_address];
		if(val == ENTRY_FREE || val == ENTRIES_FREE)
		{//Found a free entry
			return entry_address;
		}
		
		entry_address += 32; //Increments 32 bytes to next file entry
	}
	
	//If it reaches this far, it means that it has reached the end of the cluster and
	//it should check the next cluster in the chain, or if it is the last, return 0
	uint_t relative_cluster = _filesys_calc_relative_cluster(dir_address);
	uint_t next_cluster = _filesys_find_next_cluster(relative_cluster);
	
	if(next_cluster >= LAST_CLUSTER)
	{//Reached the end of the cluster chain and didn't find any free entries
		return 0;
	}
	
	uint_t next_cluster_loc = _filesys_calc_absolute_cluster_loc(next_cluster);
	
	//Recursively checks the next cluster in the chain.
	return _filesys_find_first_free_entry(next_cluster_loc);
	
}

/*
** _filesys_expand_cluster_chain - Expands the cluster chain starting at the given start
**									relative cluster number and expands it to one more
**									cluster and returns the address to the newly added
**									cluster.
*/
uint_t _filesys_expand_cluster_chain(uint_t start_cluster)
{
	uint_t prev_cluster = start_cluster;
	uint_t next_cluster = _filesys_find_next_cluster(prev_cluster);
	
	while(next_cluster != LAST_CLUSTER)
	{//Continue searching through the chain for the last cluster
		prev_cluster = next_cluster;
	}

	//Gets a free cluster to expand the chain into
	next_cluster = _filesys_find_next_free_cluster();
	
	//Updates the FATs changing the old last cluster to point to the new next_cluser
	_filesys_update_fats(prev_cluster, next_cluster);
	
	//Updates the FATs, setting the next_cluster as the end
	_filesys_update_fats(next_cluster, LAST_CLUSTER);
	
	return next_cluster;
}

/*
** _filesys_find_next_free_cluster - Goes through the FAT and locates the first available
**										cluster that is free and returns the relative
**										cluster number. Returns 0 if no free cluster found
*/
uint_t _filesys_find_next_free_cluster(void)
 {
 	uint_t fat_size = boot_sector.table_size_32 * boot_sector.bytes_per_sector / 4;
 	uint_t *fat = (uint_t*)(filesystem+fat_start_loc);
 	
 	
 	for(uint_t i = 2; i < fat_size; i++)
 	{
 		if(*(fat+i) == 0x0000)
 		{//Found a free cluster
 			return i;
 		}
 	}
 	
 	//Reached end of FAT without finding a free cluster, return 0
 	return 0;
 
}

/*
** _filesys_write_file_entry - Writes a file entry at the given entry location using
**								the provided filename, attributes, and cluster number
*/
void _filesys_write_file_entry(uint_t new_entry_loc, char* filename, ubyte_t attributes, 
 								uint_t new_file_cluster)
{
 	byte_t *entry = filesystem+new_entry_loc;
 	
 	ushort_t cluster_low = new_file_cluster & 0x00FF;
 	ushort_t cluster_hi = new_file_cluster >> 0x8 & 0x00FF;
 	
 	_kmemcpy(entry, (byte_t*)filename, 11);	//filename
 	*(entry+11) = attributes;				//attributes
 	*(entry+12) = 0x0;						//reserved_NT
 	*(entry+13) = 0x0;						//create_time_milli
 	*(entry+14) = 0x00;						//create_time
 	*(entry+16) = 0x00;						//create_date
 	*(entry+18) = 0x00;						//last_access_date
 	*(entry+20) = cluster_hi;				//first_cluster_hi
 	*(entry+22) = 0x00;						//write_time
 	*(entry+24) = 0x00;						//write_date
 	*(entry+26) = cluster_low;				//first_cluster_low
 	*(entry+28) = 0x0000;					//file_size
}
 
/*
** _filesys_update_fats - Updates the relative cluster entry in all of the FATS in the FS
**
*/
void _filesys_update_fats(uint_t relative_cluster, uint_t value)
{
	uint_t num_fats = boot_sector.num_fats;
	uint_t fat_size = boot_sector.table_size_32 * boot_sector.bytes_per_sector / 4;

	//Finds the first entry in the first FAT
	uint_t *update_fat_entry = (uint_t*)(filesystem+(fat_start_loc +(relative_cluster * 4)));

	for(uint_t i = 0; i < num_fats; i++)
	{
		update_fat_entry = update_fat_entry+(i * fat_size); //Moves to next FAT

		*update_fat_entry = *update_fat_entry | value;
	}
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _filesys_make_file - Creates a file at the given path location within
**						the filesystem using the given attributes and returns the file 
**						entry for the newly created file
**
*/
uint_t _filesys_make_file(char* path, ubyte_t attributes, file_entry_t* new_file)
{
	//Splits the path into the path-to-parent directory and the new file name
	uint_t path_len = _kstrlen(path);
	char* filename_start = path+path_len - 1;
	
	while(*filename_start != '/')
	{//while it has not found the last '/' in the path
		if(filename_start == path)
		{//It reached the beginning of the path
			break;
		}
		filename_start--;
	}
	
	uint_t parent_path_len = filename_start - path + 1;
	char parent_path[parent_path_len];
	
	//Copies the parent_path from path into parent_path
	_kmemcpy((byte_t*)parent_path, (byte_t*)path, parent_path_len);
	parent_path[parent_path_len-1] = '\0'; //Null terminates the parent_path
	
	filename_start++; //Moves the filename start loc forward 1 so it doesn't include the '/'
	char filename[12];
	_filesys_convert_to_shortname(filename_start, filename);
	
	//Checks to see if the new_file already exists, and if so, it will return FAILURE
	if(_filesys_find_file(path, new_file, 0) == 0)
		return FAILURE;
	
	//Locates the parent directory file entry
	file_entry_t parent_dir[1];
	_filesys_find_file(parent_path, parent_dir, 0);
	
	//Finds the first empty file entry slot in the parent directory
	uint_t entry_cluster = parent_dir->first_cluster_hi << 8 | parent_dir->first_cluster_low;
	uint_t dir_address = _filesys_calc_absolute_cluster_loc(entry_cluster);
	uint_t new_entry_loc = _filesys_find_first_free_entry(dir_address);
	
	if(new_entry_loc == 0)
	{//No new entry location was able to be found, needs to expand the cluster chain
		new_entry_loc = _filesys_expand_cluster_chain(entry_cluster);
	}
	
	//Finds the next free cluster for the new file contents to be stored
	uint_t new_file_cluster = _filesys_find_next_free_cluster();
	
	if(new_file_cluster == 0) return FAILURE; //Failed to create new file
	
	//Writes the new entry
	_filesys_write_file_entry(new_entry_loc, filename, attributes, new_file_cluster);
	
	//Looks for the newly created entry and returns FAILURE if it isn't found
	if(_filesys_find_file(path, new_file, 0) == 1)
		return FAILURE;
	
	//Updates the FAT to say the file's cluster is not free
	_filesys_update_fats(new_file_cluster, LAST_CLUSTER);
	
	//Clears the cluster of the new file data to all 0's
	byte_t *file_data = filesystem+_filesys_calc_absolute_cluster_loc(new_file_cluster);
	_kmemclr(file_data, cluster_size);
	
	return SUCCESS;
}

/*
** _filesys_find_file - Given a file path and a folder address, it will find the file in 
**						the filesystem and copy the file_entry into the given file entry's
**						memory.
**
**						PATH Format: /folder1/folder2/<filename>
**
**						IF the folder_address is set to 0, it will default to using the
**						Root Directory
**
**  					 Return SUCCESS and FAILURE
**
*/
uint_t _filesys_find_file(char* path, file_entry_t* file, uint_t dir_address)
{	
	//Splits path into head (first folder name) and Tail (rest of path)
	//Split is performed by finding first 2nd "/" and converts it to \0 
	//				(because path should start with "/")
	//	unless it reaches \0 before finding a "/". 
	char* path_tail = path + 1;
	
	//Finds the 2nd "/" in the path
	while(*path_tail != '\0')
	{ //While it has not reached end of path
		if(*path_tail == '/')
		{//It has found a "/"
			break;
		}
		 path_tail++;
	}
	
	//Calculates HEAD and TAIL lengths
	uint_t head_len = path_tail - path;
	
	//Takes HEAD of path and stores it in filename
	char full_filename[head_len]; //HEAD
	_kmemcpy(full_filename, path+1, head_len);
	full_filename[head_len-1] = '\0';
	
	//Converts the filename to shortname equivalent 
	char filename[12];
	_filesys_convert_to_shortname(full_filename, filename);
	
	if(dir_address == 0)
	{ //USE Root Directory
		dir_address = data_start_sector * boot_sector.bytes_per_sector;
	}
	
	uint_t entries_size = MAX_DIRECTORY_SIZE;
	file_entry_t entries[entries_size];
	uint_t num_entries = _filesys_readdir(entries, entries_size, dir_address);
	
	for(int i = 0; i < num_entries; i++)
	{
		if(_kstrcmp(entries[i].name, filename) == 0)
		{ // FOUND the entry we were looking for!
			//Copy that file entry into the file memory
			_kmemcpy((byte_t*)file, (byte_t*)(entries+i), sizeof(file_entry_t));
			
			if(_kstrcmp(path_tail, "") == 0)
			{ // AT the final folder in path, return back succes
				return SUCCESS;
			}
			
			//Found folder, move to next part of path!
			uint_t entry_cluster = entries[i].first_cluster_hi << 8 |
														entries[i].first_cluster_low;
			uint_t next_address = _filesys_calc_absolute_cluster_loc(entry_cluster);
			return _filesys_find_file(path_tail, file, next_address); //SUCCESS 
		}
	}
	
	return FAILURE;
}

/*
** _filesys_readdir -  finds a directory at the given address and reads the given number
**						 of entries entries passed into the function, within the directory
**						 and stores each entry in the given file entry array.
**
**						Returns the number of entries that were added to the array
*/
uint_t _filesys_readdir(file_entry_t *entries, uint_t entries_size, uint_t dir_address)
{
	//Cheats by using _filesys_readfile to read the byte data of the directory entries
	//which can then be parsed without dealing with cluster chains.
	uint_t data_size  = entries_size * sizeof(file_entry_t);
	byte_t directory_data[data_size];
	_kmemclr(directory_data, data_size); //Clears the memory so no extra entries get "read"
	_filesys_readfile(directory_data, dir_address, 0, data_size);
	
	uint_t entry_count = 0;
	uint_t data_offset = 0;
	while(directory_data[data_offset] != ENTRIES_FREE && entry_count <= entries_size)
	{ // While there are valid entries and it has gotten less than entries_size
	
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
		file.name[11] = '\0';
		
		
		//File entry valid, add to entries array
		if(file.name[0] != ENTRY_FREE && file.name[0] != ENTRIES_FREE && 
			(file.attributes & ATTR_LONG_NAME) != ATTR_LONG_NAME)
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
