/*
** SCCS ID:	@(#)syscall.h	1.1	4/9/14
**
** File:	syscall.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	System call module definitions
*/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "common.h"

/*
** General (C and/or assembly) definitions
*/

// system call codes

#define	SYS_exit		0
#define	SYS_readch		1
#define	SYS_writech		2
#define	SYS_writes		3
#define	SYS_spawn		4
#define	SYS_sleep		5
#define	SYS_getprio		6
#define	SYS_setprio		7
#define	SYS_getpid		8
#define	SYS_getppid		9
#define	SYS_gettime		10
#define	SYS_c_putchar_at	11
#define SYS_readfile	12
#define SYS_readdir		13
#define SYS_mkdir		14
#define SYS_syncspawn	15
#define SYS_writefile	16
#define SYS_delete		17

// number of "real" system calls

#define	N_SYSCALLS	18

// dummy system call code to test the syscall ISR

#define	SYS_bogus	0xbadc0de

// system call interrupt vector number

#define	INT_VEC_SYSCALL	0x80

#ifndef __SP_ASM__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

#ifdef __SP_KERNEL__

#include "queue.h"

#include <x86arch.h>

/*
** OS only definitions
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

extern queue_t _sleeping;      // sleeping processes

/*
** Prototypes
*/

/*
** initializes all syscall-related data structures
*/

void _sys_init( void );

#endif

#endif

#endif
