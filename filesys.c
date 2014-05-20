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
	if(_kstrcmp(filename, ".") == 0 || _kstrcmp(filename, "..") == 0)
	{
		uint_t len = _kstrlen(filename);
		shortname[0] = '.';
		if(len == 2) shortname[1] = '.';
		
		for(int i = 2; i < 11; i ++) shortname[i] = ' ';
		shortname[11] = '\0';
		
		return;
	}

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
	while(entry_address < end_of_cluster)
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
	
	while(next_cluster < LAST_CLUSTER)
	{//Continue searching through the chain for the last cluster
		prev_cluster = next_cluster;
		next_cluster = _filesys_find_next_cluster(prev_cluster);
	}

	//Gets a free cluster to expand the chain into
	next_cluster = _filesys_find_next_free_cluster();
	
	//Updates the FATs changing the old last cluster to point to the new next_cluser
	_filesys_update_fats(prev_cluster, next_cluster);
	
	//Updates the FATs, setting the next_cluster as the end
	_filesys_update_fats(next_cluster, LAST_CLUSTER);
	
	//Clear memory of the next cluster
	byte_t *file_data = filesystem+_filesys_calc_absolute_cluster_loc(next_cluster);
	_kmemclr(file_data, cluster_size);
	
	return next_cluster;
}

/*
** _filesys_shrink_cluster_chain - Shrinks the cluster chain down to a single cluster
**									freeing up all the other clusters in the chain
** 
*/
void _filesys_shrink_cluster_chain(uint_t start_cluster)
{
	uint_t current_cluster = start_cluster;
	
	while(current_cluster < LAST_CLUSTER)
	{
		uint_t next_cluster = _filesys_find_next_cluster(current_cluster);
		
		//Frees the current_cluster, if it isn't the start_cluster
		if(current_cluster != start_cluster)
			_filesys_update_fats(current_cluster, 0x0000);
		else
			_filesys_update_fats(current_cluster, LAST_CLUSTER);
			
		current_cluster = next_cluster;
	}
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

		*update_fat_entry = value;
	}
}

/*
** _filesys_update_file_size - Updates the file's file size in it's entry by locating the 
**								entry in the filesystem using the given filename at the
**								parent directory location and updates it to the given
**								data length
*/
void _filesys_update_file_size(char *filename, file_entry_t *parent_dir, uint_t data_len)
{
	uint_t current_cluster = parent_dir->first_cluster_hi << 16 | parent_dir->first_cluster_low;
	
	while(current_cluster < LAST_CLUSTER)
	{
		uint_t current_cluster_loc = _filesys_calc_absolute_cluster_loc(current_cluster);
		byte_t *entry = filesystem + current_cluster_loc;
		
		byte_t *end_of_cluster = entry + cluster_size;
		while(entry < end_of_cluster)
		{//Searches for entry matching filename
			char name[12] = {'\0'};
			_kmemcpy((byte_t*)name, (byte_t*)entry, 11);
			
			if(_kstrcmp(name, filename) == 0)
			{//Update filesize in the last 4 bytes of the 32 byte entry
				*(uint_t*)(entry+28) = data_len;
				
				return;
			}
			
			entry += 32;
		}
		current_cluster = _filesys_find_next_cluster(current_cluster);
	}
}

/*
** _filesys_delete_file_entry - Sets the first byte of the file entry in the file system
**								to ENTRY_FREE
*/
void _filesys_delete_file_entry(char *path, file_entry_t *file)
{
	//Gets the cluster number for the new directory's parent
	char *parent_path_end = path+_kstrlen(path) - 1;
	while(*(--parent_path_end) != '/') {}
	
	uint_t parent_path_len = parent_path_end - path + 1;
	char parent_path[parent_path_len];
	_kmemcpy((byte_t*)parent_path,(byte_t*)path, parent_path_len);
	parent_path[parent_path_len - 1] = '\0';
	file_entry_t parent_dir;
	_filesys_find_file(parent_path, &parent_dir, 0);
	
	uint_t current_cluster = parent_dir.first_cluster_hi << 16 | parent_dir.first_cluster_low;
	
	while(current_cluster < LAST_CLUSTER)
	{
		uint_t current_cluster_loc = _filesys_calc_absolute_cluster_loc(current_cluster);
		byte_t *entry = filesystem + current_cluster_loc;
		
		byte_t *end_of_cluster = entry + cluster_size;
		while(entry < end_of_cluster)
		{//Searches for entry matching filename
			char name[12] = {'\0'};
			_kmemcpy((byte_t*)name, (byte_t*)entry, 11);
			
			if(_kstrcmp(name, file->name) == 0)
			{//Update entry's first byte to ENTRY_FREE
				*(entry) = ENTRY_FREE;
				
				return;
			}
			
			entry += 32;
		}
		current_cluster = _filesys_find_next_cluster(current_cluster);
	}
}

/*
** _filesys_convert_shortname_to_normal - converts a shortname file name to a normal
**											filename
*/
void _filesys_convert_shortname_to_normal(char *shortname, char *converted)
{
	//Special case of "." and ".." files in the filesystem
	if(_kstrcmp(shortname, ".          ") == 0 || _kstrcmp(shortname, "..         ") == 0)
	{
		converted[0] = '.';
		converted[1] = '\0';
		
		if(shortname[1] == '.')
		{//It is a ".." file
			converted[1] = '.';
			converted[2] = '\0';
		}

		return;
	}

	int index = 0;
	for(int i = 0; i < 8; i++)
	{
		if(shortname[i] == ' ')
			continue;
			
		converted[index] = shortname[i];
		index++;
	}
	
	//Checks if the next shortname character is a space
	//If it is, then that means there is no extension
	if(shortname[8] == ' ')
		converted[index] = ' ';
	else
		converted[index] = '.';
	index++;
	
	for(int i = 8; i < 12; i++)
	{
		if(shortname[i] == ' ')
			break;
			
		converted[index] = shortname[i];
		index++;
	}
}

/*
** _filesys_calc_offset_loc - given a cluster number and an offset, calculate the relative
**								location in the filesystem at which the offset should
**								go point at
*/
uint_t _filesys_calc_offset_loc(uint_t relative_cluster, uint_t offset)
{
	uint_t current_cluster_loc = _filesys_calc_absolute_cluster_loc(relative_cluster);
	uint_t next_cluster = _filesys_find_next_cluster(relative_cluster);
	
	while(offset > cluster_size)
	{
		if(next_cluster == LAST_CLUSTER)
		{//The last cluster has been reached before offset is smaller than the cluster
			return current_cluster_loc + cluster_size;
		}
		
		current_cluster_loc = _filesys_calc_absolute_cluster_loc(next_cluster);
		next_cluster = _filesys_find_next_cluster(next_cluster);
		offset -= cluster_size;
	}
	
	return current_cluster_loc + offset;
}

/*
** PUBLIC FUNCTIONS
*/

/* 
** _filesys_delete - Deletes a file within the filesystem. If the file is a directory, it
**						will delete every file within the directory as well
*/
void _filesys_delete(char *path)
{
	file_entry_t file;
	if(_filesys_find_file(path, &file, 0) == 1)
	{//Couldn't find file to delete, return
		return;
	}
	
	c_printf("filename: %s\n", file.name);
	
	//Special case check for root
	if((file.first_cluster_hi << 16 | file.first_cluster_low) == 0x02)
	{
		return;
	}
	
	//Special case check for the . and .. "directories"
	if(_kstrcmp(file.name, ".          ") == 0 || _kstrcmp(file.name, "..         "))
	{
		_filesys_delete_file_entry(path, &file);
		return;
	}
	
	int dir_cluster = file.first_cluster_hi << 16 | file.first_cluster_low;
	
	if(_filesys_is_directory(file) == 0)
	{//It is a directory
		int dir_address = _filesys_calc_absolute_cluster_loc(dir_cluster);
		file_entry_t entries[MAX_DIRECTORY_SIZE];
		
		int num_entries = _filesys_readdir(entries, MAX_DIRECTORY_SIZE, dir_address);
		int path_len = _kstrlen(path);
		
		for(int i = 0; i < num_entries; i++)
		{//For each entry in the directory, add the file to the path and delete
			char normal_filename[13] = { '\0' };
			_filesys_convert_shortname_to_normal(entries[i].name, normal_filename);
			
			char extended_path[path_len + 12];
			_kmemcpy(extended_path, path, path_len);
			_kmemcpy(extended_path+path_len+1, normal_filename, 13); 
			extended_path[path_len] = '/';
			
			c_printf("Extended path: %s", extended_path);
			
			_filesys_delete(extended_path);
		}
	}
	
	//Deletes a single (non-directory) file
	//Clears fats for all cluster in file expect first
	_filesys_shrink_cluster_chain(dir_cluster);
	
	//Updates the fats to clear the first cluster of the file	
	_filesys_update_fats(dir_cluster, 0x0000);
	
	//Sets first byte of file entry to ENTRY_FREE
	_filesys_delete_file_entry(path, &file);
	
}

/*
** _filesys_is_directory - Determines if the given file is a directory or not.
**                          Returns SUCCESS if it is, FAILURE otherwise
*/
uint_t _filesys_is_directory(file_entry_t file)
{
	if((file.attributes & ATTR_DIRECTORY) == ATTR_DIRECTORY)
    {
        return SUCCESS;
    }
        
    return FAILURE;
}

/*
** _filesys_write_file - Writes the given data of the given length to the given file
**							If there is data in the file already, clears it
**
**							Returns SUCCESS if write is successful, FAILURE otherwise
*/
uint_t _filesys_write_file(char* path, byte_t *data, uint_t data_len)
{
	file_entry_t file[1];
	
	if(_filesys_find_file(path, file, 0) == 1)
	{//Failed to find the file to be written to
		return FAILURE;
	}
	
	//Shrinks the file down to 1 cluster and clears the memory before writing
	uint_t start_cluster = file->first_cluster_hi << 16 | file->first_cluster_low;
	uint_t start_cluster_loc = _filesys_calc_absolute_cluster_loc(start_cluster);
	_filesys_shrink_cluster_chain(start_cluster);
	_kmemclr(filesystem+start_cluster_loc, cluster_size);
	
	//Writes the data
	uint_t data_index = 0;
	uint_t remaining_data = data_len;
	uint_t current_cluster_loc = start_cluster_loc;
	
	while(remaining_data > 0)
	{//While there is still data
		if(remaining_data < cluster_size)
		{//The remaining data fits in the current cluster
			_kmemcpy(filesystem+current_cluster_loc, data+data_index, remaining_data);
						
			data_index += remaining_data;
			remaining_data = 0;
			continue;
		}
		
		//Requires more than 1 cluster to write data
		_kmemcpy(filesystem+current_cluster_loc, data+data_index, cluster_size);
		
		data_index += cluster_size;
		remaining_data -= cluster_size;
		
		//Gets next cluster to write to (expands cluster chain by 1)
		uint_t current_cluster = _filesys_expand_cluster_chain(start_cluster);
		current_cluster_loc = _filesys_calc_absolute_cluster_loc(current_cluster);
	}
	
	//Gets the parent_directory
	char *parent_path_end = path+_kstrlen(path) - 1;
	while(*(--parent_path_end) != '/') {}
	
	uint_t parent_path_len = parent_path_end - path + 1;
	char parent_path[parent_path_len];
	_kmemcpy((byte_t*)parent_path,(byte_t*)path, parent_path_len);
	parent_path[parent_path_len - 1] = '\0';
	file_entry_t parent_dir;
	

	if(_filesys_find_file(parent_path, &parent_dir, 0) == FAILURE)
	{
		return FAILURE;
	}
	
	//Updates the file entry's file size
	_filesys_update_file_size(file->name, &parent_dir, data_len);
	
	return SUCCESS;
}

/*
** _filesys_make_dir - Makes a new directory at the given path and sets the new_dir to 
**						the newly created file entry for the new directory.
**
**						Returns SUCCESS or FAILURE
*/
uint_t _filesys_make_dir(char* path, file_entry_t* new_dir)
{
	if(_filesys_make_file(path, ATTR_DIRECTORY, new_dir) == FAILURE) return FAILURE;
	
	//Gets the cluster number for the new directory
	uint_t new_dir_cluster = new_dir->first_cluster_hi << 16 | new_dir->first_cluster_low;
	uint_t new_dir_cluster_loc = _filesys_calc_absolute_cluster_loc(new_dir_cluster);
	
	//Gets the cluster number for the new directory's parent
	char *parent_path_end = path+_kstrlen(path) - 1;
	while(*(--parent_path_end) != '/') {}
	
	uint_t parent_path_len = parent_path_end - path + 1;
	char parent_path[parent_path_len];
	_kmemcpy((byte_t*)parent_path,(byte_t*)path, parent_path_len);
	parent_path[parent_path_len - 1] = '\0';
	file_entry_t parent_dir[1];
	_filesys_find_file(parent_path, parent_dir, 0);
	
	uint_t parent_cluster = parent_dir->first_cluster_hi << 16 | parent_dir->first_cluster_low;
	
	//Creates the "." file
	_filesys_write_file_entry(new_dir_cluster_loc, ".          ", 
								ATTR_DIRECTORY, new_dir_cluster);
	
	//Creates the ".." file
	_filesys_write_file_entry(new_dir_cluster_loc+32, "..         ", 
								ATTR_DIRECTORY, parent_cluster);
	
	return SUCCESS;
}

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
	if(_filesys_find_file(path, new_file, 0) == SUCCESS)
		return FAILURE;
	
	//Locates the parent directory file entry
	file_entry_t parent_dir[1];
	if(_filesys_find_file(parent_path, parent_dir, 0) == FAILURE)
		return FAILURE;
	
	//Finds the first empty file entry slot in the parent directory
	uint_t entry_cluster, dir_address;
	entry_cluster = parent_dir->first_cluster_hi << 16 | parent_dir->first_cluster_low;
	dir_address = _filesys_calc_absolute_cluster_loc(entry_cluster);
	
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
	if(_filesys_find_file(path, new_file, 0) == FAILURE)
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
uint_t _filesys_find_file(const char* path, file_entry_t* file, uint_t dir_address)
{	
	//First checks to see if root directory, and if soo, it generates a fake file entry
	// with the root directories start cluster
	if(_kstrcmp(path, "") == 0 || _kstrcmp(path, "/") == 0)
	{
		uint_t root_loc = data_start_sector * boot_sector.bytes_per_sector;
		_filesys_convert_to_shortname("/", file->name);
		file->first_cluster_hi = 0x00;
		file->first_cluster_low = _filesys_calc_relative_cluster(root_loc);
		file->attributes = ATTR_DIRECTORY;
		return SUCCESS;
	}

	char *curr, *end_curr;
	curr = path;
	end_curr = path+1;
	
	//Set's root address as the starting dir_address
	dir_address = data_start_sector * boot_sector.bytes_per_sector;
	
	//Dissects each part of the path, from the beginning, until it has reached the end
	while(*curr != '\0')
	{
		//Moves end_curr to the next "/" in the path
		while(*end_curr != '/' && *end_curr != '\0') { end_curr++; }
		
		uint_t name_len = end_curr - curr;
		char name[name_len];
		
		_kmemcpy(name, curr+1, name_len);
		name[name_len- 1] = '\0';
		
		//Converts the filename to shortname equivalent 
		char filename[12];
		_filesys_convert_to_shortname(name, filename);
		
		uint_t entries_size = MAX_DIRECTORY_SIZE;
		file_entry_t entries[entries_size];
		uint_t num_entries = _filesys_readdir(entries, entries_size, dir_address);
		
		//Loops over each entry
		for(int i = 0; i < num_entries; i++)
		{
			if(_kstrcmp(entries[i].name, filename) == 0)
			{ // FOUND the entry we were looking for!
				//Copy that file entry into the file memory
				_kmemcpy((byte_t*)file, (byte_t*)(entries+i), sizeof(file_entry_t));
			
				break;
			}
		}
		
		if(_kstrcmp(file->name, filename) != 0)
		{//FAILED to find current file
			return FAILURE;
		}
		
		//Updates the dir_address to the subdirectory's address
		uint_t file_cluster = file->first_cluster_hi << 16 | file->first_cluster_low;
		dir_address = _filesys_calc_absolute_cluster_loc(file_cluster);
		
		curr = end_curr++;
	}

	return SUCCESS;
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
	uint_t current_cluster = _filesys_calc_relative_cluster(dir_address);
	uint_t entry_count = 0;
	
	while(current_cluster < LAST_CLUSTER)
	{//Continues through directory reading entries until it reaches end of cluster chain
		byte_t *entry = filesystem+dir_address;
		byte_t *end_of_cluster = entry + cluster_size;
		
		while(entry < end_of_cluster)
		{
			//Parses a file entry
			file_entry_t file = 
			{
				.name = {'\0'},
				.attributes = *(entry+11),
				.reserved_NT = *(entry+12),
				.create_time_milli = *(entry+13),
				.create_time = *(ushort_t*)(entry+14),
				.create_date = *(ushort_t*)(entry+16),
				.last_access_date = *(ushort_t*)(entry+18),
				.first_cluster_hi = *(ushort_t*)(entry+20),
				.write_time = *(ushort_t*)(entry+22),
				.write_date = *(ushort_t*)(entry+24),
				.first_cluster_low = *(ushort_t*)(entry+26),
				.file_size = *(uint_t*)(entry+28)
			};
			
			_kmemcpy(file.name, entry, 11);
			
			//Special Case:
			//				IF cluster number is 0x0000, then set to root at 0x0002
			uint_t cluster = file.first_cluster_hi << 16 | file.first_cluster_low;
			if(cluster == 0x0000)
			{//Should use root
				uint_t root_loc = data_start_sector * boot_sector.bytes_per_sector;
				file.first_cluster_hi = 0x00;
				file.first_cluster_low = _filesys_calc_relative_cluster(root_loc);
			}
			
			//File entry valid, add to entries array (ignores long name entries)
			if(file.name[0] != (char)ENTRY_FREE && file.name[0] != (char)ENTRIES_FREE &&
				(file.attributes & ATTR_LONG_NAME) != ATTR_LONG_NAME)
			{
				entries[entry_count] = file;
				entry_count++;
			}
			
			if(entry_count == entries_size)
			{
				return entry_count;
			}
			
			entry += 32;
		}
		
		
		current_cluster = _filesys_find_next_cluster(current_cluster);
		dir_address = _filesys_calc_absolute_cluster_loc(current_cluster);
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
	
	//Calculates the location in the file the offset goes to
	uint_t filesys_offset = _filesys_calc_offset_loc(relative_cluster, offset);
	
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
