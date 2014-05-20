// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

#ifndef _MEMORY_H_
#define _MEMORY_H_

// The size of each page of memory
#define PAGE_SIZE 0x1000

// The number of entries per page table structure
#define NUM_PTES 512

#define DYNAMIC_MEM_LOW ((void*)0x300000)
#define DYNAMIC_MEM_HIGH ((void*)0xcd33d000)

/*
** _mem_init()
**
** Initialize the memory protection module
*/
void _mem_init(void);


/*
** _mem_page_frame_alloc: watermark page frame allocator
*/
physaddr_t _mem_page_frame_alloc(void);

/*
** _mem_page_frame_free: watermark page frame "deallocator"
*/
void _mem_page_frame_free(physaddr_t page);

/*
** _mem_page_table_free: deallocate a page table and all its backing page frames
*/
void _mem_page_table_free(physaddr_t pt);

/*
** _mem_map_page_onto: map the given page frame onto the given page-ligned memory address
**
** page: the page-aligned memory address onto which this frame should be mapped
** frame: the physical address of memory the page should be mapped to.
*/
void _mem_map_page_onto(void *page, physaddr_t frame);

/*
** _mem_map_page: map the given physical address of a page frame into virtual memory
** returns the address where the page is mapped.
**
** NOTE: currently this can only map one page at a time, and will panic if a page is already
** mapped in the scratch area. mapped memory must be unmapped with _mem_unmap_page when done
** being used.
*/
void *_mem_map_page(physaddr_t page);

/*
** _mem_unmap_page: unmaps the given virtual address from virtual memory
*/
void _mem_unmap_page(void *addr);

/*
** _mem_map_user_pagetab: map the page table of the current process at index 1
*/
void _mem_map_user_pagetab(physaddr_t frame);

/*
** _mem_unmap_user_pagetab: unmap the first (the userspace) page table
*/
void _mem_unmap_user_pagetab(void);

/*
** _mem_range_is_mapped: test whether the range is within the mapped pages belonging to userland
*/
_Bool _mem_range_is_mapped(void *start_address, uint_t length);

/*
** _mem_kill_overflowing_process: terminate a process that didn't leave us
** 			 enough room on its stack to store a context_t
*/
void _mem_kill_overflowing_process(void);

#endif
