/*
** SCCS ID:	@(#)types.h	1.1	4/9/14
**
** File:	types.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	General data type declarations
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** A user space structure that represents file data important for user space use
**		name, file_size, and is_directory (0 if not, 1 if it is)
*/
typedef struct file_entry_user {
	char 			name[13];
	unsigned int	file_size;
	char			is_directory;
} file_t;

// generic integer types

typedef unsigned int	uint_t;

// size-specific integer types

typedef char		int8_t;
typedef unsigned char	uint8_t;
typedef int8_t		byte_t;
typedef uint8_t		ubyte_t;

typedef short		int16_t;
typedef unsigned short	uint16_t;
typedef int16_t		short_t;
typedef uint16_t	ushort_t;

typedef int		int32_t;
typedef unsigned int	uint32_t;

typedef long		int64_t;
typedef unsigned long	uint64_t;

// user-visible process-related types

typedef	uint16_t	pid_t;		// process id
typedef uint8_t		prio_t;		// process priority

// system time

typedef uint32_t	time_t;

#ifdef __SP_KERNEL__

/*
** OS-only type declarations
*/

// status return value

typedef uint8_t		status_t;

// physical address type

typedef struct {
	void *addr;
} physaddr_t;

#endif

/*
** Globals
*/

/*
** Prototypes
*/

#endif

#endif
