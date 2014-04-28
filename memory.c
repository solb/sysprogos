// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

/*
** Name:	__page_fault_handler
**
** Description: Handles page faults
*/

#define __SP_KERNEL__

#include "common.h"
#include "memory.h"

#include "bootstrap.h"
#include "stack.h"
#include "startup.h"
#include "x86arch.h"

/*
** PRIVATE FUNCTIONS
*/

/*
** __expose_user_pages()
**
** allow access to pages for the userspace stack and the USERSPACE_SIZE bytes after the beginning of userspace code
*/
static void __expose_user_pages(void) {
	uint64_t *pt = (uint64_t *)PT_ADDRESS + ((USERSPACE_ADDRESS - N_STACKS * sizeof(stack_t)) >> 12);
	do {
		*pt |= PAGE_USER;
	}
	while(++pt < (uint64_t *)PT_ADDRESS + ((USERSPACE_ADDRESS + USERSPACE_SIZE) >> 12));
	__inv_tlb();
}

/*
** __page_fault_handler(int, int)
**
** ISR for #PF exceptions
*/
static void __page_fault_handler(int vector, int code) {
	c_printf("Page fault: code %d addr 0x%x\n", code, __get_cr2());
	__blame_and_punish();
}

/*
** PUBLIC FUNCTIONS
*/

/*
** _mem_page_frame_alloc: watermark page frame allocator
*/
physaddr_t _mem_page_frame_alloc(void) {
	static uint64_t page_counter = 0;
	void *result = DYNAMIC_MEM_LOW + (page_counter<<12);
	++page_counter;
	if (result >= DYNAMIC_MEM_HIGH) {
		_kpanic("mem", "OUT OF DYNAMIC MEMORY", FAILURE);
	}
	return (physaddr_t){result};
}

/*
** _mem_page_frame_free: watermark page frame "deallocator"
*/
void _mem_page_frame_free(physaddr_t page) {
	//I'm FREE!
}

#define SCRATCH_PAGE ((void*)0x1ff000)
static int scratch_page_mapped = 0;

/*
** _mem_map_page: map the given physical address of a page frame into virtual memory
** returns the address where the page is mapped.
**
** NOTE: currently this can only map one page at a time, and will panic if a page is already
** mapped in the scratch area. mapped memory must be unmapped with _mem_unmap_page when done
** being used.
*/
void *_mem_map_page(physaddr_t page) {
	if (scratch_page_mapped) {
		_kpanic("mem", "tried to map scratch page while already mapped", FAILURE);
	}
	scratch_page_mapped = 1;
	uint64_t *kernel_page_table = PT_ADDRESS;
	kernel_page_table[(uint64_t)SCRATCH_PAGE>>12] = (uint64_t)(page.addr) | PAGE_PRESENT;
	__inv_tlb();
	return SCRATCH_PAGE;
}

/*
** _mem_unmap_page: unmaps the given virtual address from virtual memory
*/
void _mem_unmap_page(void *addr) {
	if (addr != SCRATCH_PAGE) {
		_kpanic("mem", "tried to unmap a non-scratch page", FAILURE);
	}
	if (!scratch_page_mapped) {
		_kpanic("mem", "tried to unmap a non-mapped page", FAILURE);
	}
	uint64_t *kernel_page_table = PT_ADDRESS;
	kernel_page_table[(uint64_t)addr>>12] = 0;
	__inv_tlb();
	scratch_page_mapped = 0;
}

/*
** _mem_init()
**
** Initialize the memory protection module
*/
void _mem_init(void) {
	__install_isr(INT_VEC_PAGE_FAULT, __page_fault_handler);
	__expose_user_pages();
}
