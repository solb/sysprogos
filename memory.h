// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

#ifndef _MEMORY_H_
#define _MEMORY_H_

// The size of each page of memory
#define PAGE_SIZE 0x1000

// The number of entries per page table structure
#define NUM_PTES 512

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
void _mem_page_table_free(physaddr_t pt);
void _mem_map_page_onto(void *page, physaddr_t frame);
void *_mem_map_page(physaddr_t page);
void _mem_unmap_page(void *addr);
void _mem_map_user_pagetab(physaddr_t frame);
void _mem_unmap_user_pagetab(void);
void _mem_kill_overflowing_process(void);

#endif
