/*
** SCCS ID:	@(#)ulib.h	1.1	4/9/14
**
** File:	ulib.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:	Chris Cooper
** Contributor: Dan Johnson
** Contributor: Sol Boucher
**
** Description:	User-level library declarations
**
** These are separate from the kernel library so that user processes
** aren't trying to use code that is in the kernel's address space,
** in case anyone wants to implement real memory protection.
*/

#ifndef _ULIB_H_
#define _ULIB_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// process priorities

#define	PRIO_HIGH	0
#define	PRIO_STD	1
#define	PRIO_LOW	2

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Macros
*/

// pseudo function to convert seconds to milliseconds

#define	seconds_to_ms(n)	((n) * 1000)

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
** reads - read len-1 characters from the SIO console
**
** usage: readstr(unsigned, char *)
**
** takes: buffer length and corresponding buffer
**
** returns: number of non-null characters/bytes read
*/

unsigned reads(unsigned len, char *buf);

/*
** exit - terminate the calling process
**
** usage:	exit();
**
** does not return
*/

void exit( void );

/*
** readch - read a single character from the SIO
**
** usage:	ch = readch();
**
** blocks the calling routine if there is no character to return
**
** returns:
**      the character
*/

int readch( void );

/*
** writech - write a single character to the SIO
**
** usage:	writech(ch);
**
** returns:
**      the character that was written
*/

int writech( char ch );

/*
** writes - write a single character to the SIO
**
** usage:	writes(ch);
**
** returns:
**      the number of characters written
*/

int writes( char *str );

/*
** spawn - create a new process running a different program
**
** usage:	pid = spawn(entry);
**
** returns:
**	pid of the spawned process, or -1 on failure
*/

pid_t spawn( char *path );

/*
** sleep - put the current process to sleep for some length of time
**
** usage:	sleep(ms);
**
** if the sleep time (in milliseconds) is 0, just preempts the process;
** otherwise, puts it onto the sleep queue for the specified length of
** time
*/

void sleep( uint_t ms );

/*
** getprio - retrieve the priority of this process
**
** usage:	prio = getprio(void);
**
** returns:
**      the process' priority
*/

prio_t getprio( void );

/*
** setprio - change the priority of this process
**
** usage:	oldprio = setprio(prio);
**
** returns:
**      the old priority of the process
*/

prio_t setprio( prio_t prio );

/*
** getpid - retrieve the PID of the current process
**
** usage:	pid = getpid();
**
** returns:
**      the process' pid
*/

pid_t getpid( void );

/*
** getppid - retrieve the parent PID of the current process
**
** usage:	pid = getppid();
**
** returns:
**      the process' parent's pid
**
** note: if the original parent of this process has exited, this process
** will have been inherited by the system init process, whose PID will
** be returned
*/

pid_t getppid( void );

/*
** gettime - retrieve the current system time
**
** usage:	time = gettime();
**
** returns:
**      the current system time
*/

time_t gettime( void );

/*
** c_putchar_at - syscall for debug printing
*/

void c_putchar_at( unsigned int x, unsigned int y, unsigned int c );

/*
** syncspawn - defer flow of control to a new process
**
** usage:	pid = spawn(char *);
**
** takes:
**  string containing path to binary
**
** returns:
**	pid of the completed process, or -1 on failure
*/

pid_t syncspawn( char *path );

/*
** bogus - a bogus system call, for testing our syscall ISR
**
** usage:	bogus();
*/

void bogus( void );

/*
** readfile - reads data from a file in the filesystem
**
** usage:	num_bytes_read = readfile(char *path, int offset, void *buff, int count)
**
** returns:
**		the number of bytes read
*/
int readfile(char *path, int offset, void *buff, int count);

/*
** readdir - Reads the contents of a directory
**
** usage:	num_entries_read = readdir(char* path, void *buff, int count);
**
** returns:
**	the number of files read in the directory. Returns -1 if the read failed
*/
int readdir(char *path, void *buff, int count);

/*
** _sys_mkdir - Creates a directory of a given path
**
** usage:	int success = mkdir(char* path);
**
** returns:
**	 Returns SUCCESS or FAILURE of creation of directory
*/
int mkdir(char *path);

/*
** _sys_writefile - creates a file in the filesystem and writes data to it
**
** usage:	int writefile(char* path, void *buff, int count);
**
** returns:
**	SUCCESS or FAILURE
*/
int writefile(char* path, void *buff, int count);

/*
** _sys_delete - Deletes a file in the filesystem. If it is a directory, it will delete
**					everything within the directory
**
** implements:	void delete(char* path);
**
*/
void delete(char *path);

#endif

#endif
