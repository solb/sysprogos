// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "common.h"

// The size of the userspace memory, in bytes (excluding the stacks)
#define USERSPACE_SIZE	0x10000

/*
** _mem_init()
**
** Initialize the memory protection module
*/
void _mem_init(void);

typedef struct {
	void *start;
	ptrdiff_t length;
	uint32_t type;
	uint32_t acpi3_attr;
} e820_entry_t;

#endif
