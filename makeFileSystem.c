/*
** makeFileSystem.c
**
** Generates a filesystem containing the files that have been specified in the command
** line arguments
**
** Christopher Cooper
**
*/ 
#include "filesys.h"
#include "filesys.c"
#include "types.h"
#include <stdio.h>
#include <string.h>

#define FILESYSTEM	"filesystem.img"

int main(int argc, char **argv)
{
	//Generates the file 
	fprintf(stdout, "Generating filesystem image...\n");
	
	char ddstr[200] = {'\0'};
	strcat(ddstr, "dd if=/dev/zero of=");
	strcat(ddstr, FILESYSTEM);
	strcat(ddstr, " bs=300K count=1");
	
	char mkdosstr[120] = {'\0'};
	strcat(mkdosstr, "mkdosfs -F 32 ");
	strcat(mkdosstr, FILESYSTEM);
	
	system(ddstr);
	system(mkdosstr);
	
	fprintf(stdout, "Initializing filesystem driver...\n");
	_filesys_init(FILESYSTEM);

	fprintf(stdout, "Finished initializing.\n\nWriting files to filesystem...\n");
	for(int i = 1; i < argc; i++)
	{
		fprintf(stdout, "\tOpening file: %s\n", argv[i]);
		FILE *file = fopen(argv[i] , "r");
		
		if(file == NULL)
		{
			perror("\tError while opening the file: ");
			fprintf(stderr, "\t\tSkipping file\n\n");
			continue;
		}
		
		char *path = argv[i]+strlen(argv[i]);
		//Loops backward through argv[i] to find last '/' or until it reaches the beginning
		while(*path != '/' && path != argv[i]) { path--; }
		if(*path == '/') path++;
		
		char file_path[80] = { '\0' };
		strcat(file_path, "/");
		strcat(file_path, path);
		
		file_entry_t new_file;
		
		fprintf(stdout, "\tCreating new file in filesystem at path: %s\n", file_path);
		if(_filesys_make_file(file_path, ATTR_ARCHIVE, &new_file) == 1)
		{
			fprintf(stderr, "\tERROR creating file in filesystem! Skipping...\n\n");
			continue;
		}
		fprintf(stdout, "\tSuccessfully created new file.\n");
		
		fprintf(stdout, "\tWriting contents of file...\n");
		fseek(file, 0L, SEEK_END);
		uint_t data_len = ftell(file);
		
		fseek(file, 0L, SEEK_SET);
		byte_t data[data_len];
		uint_t index = 0;
		byte_t b;
		while( ( b = fgetc(file) ) != EOF )
		{
			data[index++] = b;
		}
			
		_filesys_write_file(file_path, data, data_len);
		fprintf(stdout, "\tFinished writing contents of file.\n");
		
		fprintf(stdout, "\tSuccessfully wrote file to filesystem\n\n");
		fclose(file);
	}
	
	fprintf(stdout, "Finished writing files to filesystem.\n");
}