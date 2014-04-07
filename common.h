/*
** SCCS ID:	%W%	%G%
**
** File:	common.h
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Standard includes needed in all C source files
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef __SP_ASM__

// only pull these in if we're not in assembly language

#include "defs.h"
#include "types.h"

#include "c_io.h"
#include "support.h"

#ifdef __SP_KERNEL__

// OS needs the kernel library headers and the system headers

#include "system.h"
#include "klib.h"

#else

// User code needs only the user library headers

#include "ulib.h"

#endif

#endif

#endif
