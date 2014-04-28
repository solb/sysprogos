// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

#ifndef _MEMORY_H_
#define _MEMORY_H_

// The size of the userspace memory, in bytes (excluding the stacks)
#define USERSPACE_SIZE	0x10000

#define DYNAMIC_MEM_LOW ((void*)0x100000)
#define DYNAMIC_MEM_HIGH ((void*)0xcd33d000)

/*
** _mem_init()
**
** Initialize the memory protection module
*/
void _mem_init(void);


physaddr_t _mem_page_frame_alloc(void);
void _mem_page_frame_free(physaddr_t page);
void *_mem_map_page(physaddr_t page);
void _mem_unmap_page(void *addr);

#endif
