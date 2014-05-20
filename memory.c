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
#include "scheduler.h"

/*
** PRIVATE FUNCTIONS
*/

/*
** __page_fault_handler(int, int)
**
** ISR for #PF exceptions
*/
static void __page_fault_handler(int vector, int code) {
	c_printf("Page fault: code %d addr 0x%x rip 0x%x\n", code, __get_cr2(), _current->context->rip);
	__blame_and_punish();
}

/*
** PUBLIC FUNCTIONS
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

void _mem_page_frame_free(physaddr_t page) {
	//I'm FREE!
}

void _mem_page_table_free(physaddr_t pt) {
	uint64_t *entries = _mem_map_page(pt);
	for(unsigned index = 0; index < NUM_PTES; ++index) {
		if(entries[index] & PAGE_PRESENT) {
			_mem_page_frame_free((physaddr_t){(void *)(entries[index] & ~0xfff)});
		}
	}
	_mem_unmap_page(entries);
	_mem_page_frame_free(pt);
}

void _mem_map_page_onto(void *page, physaddr_t frame) {
	uint64_t test = (uint64_t)page & ((1<<12)-1);
	if (test) {
		_kpanic("mem", "cannot map page onto not page-aligned address", FAILURE);
	}
	uint64_t *kernel_page_table = (uint64_t *)PT_ADDRESS;
	uint64_t table_index = ((uint64_t)page)>>12;
	if (table_index >= 512) {
		_kpanic("mem", "cannot map page onto address outside kernel page table.", FAILURE);
	}
	kernel_page_table[table_index] = (uint64_t)(frame.addr) | PAGE_PRESENT;
	__inv_tlb();
}

#define SCRATCH_PAGE ((void*)0x1ff000)
static int scratch_page_mapped = 0;

void *_mem_map_page(physaddr_t page) {
	if (scratch_page_mapped) {
		_kpanic("mem", "tried to map scratch page while already mapped", FAILURE);
	}
	scratch_page_mapped = 1;
	_mem_map_page_onto(SCRATCH_PAGE, page);
	return SCRATCH_PAGE;
}

void _mem_unmap_page(void *addr) {
	if (addr != SCRATCH_PAGE) {
		_kpanic("mem", "tried to unmap a non-scratch page", FAILURE);
	}
	if (!scratch_page_mapped) {
		_kpanic("mem", "tried to unmap a non-mapped page", FAILURE);
	}
	uint64_t *kernel_page_table = (uint64_t *)PT_ADDRESS;
	kernel_page_table[(uint64_t)addr>>12] = 0;
	__inv_tlb();
	scratch_page_mapped = 0;
}

void _mem_map_user_pagetab(physaddr_t frame) {
	uint64_t *pdt_entry = _mem_map_page((physaddr_t){(uint64_t *)PDT_ADDRESS});
	pdt_entry[1] = (uint64_t)frame.addr | PAGE_PRESENT | PAGE_RW | PAGE_USER;
	_mem_unmap_page(pdt_entry);
}

void _mem_unmap_user_pagetab(void) {
	uint64_t *pdt_entry = _mem_map_page((physaddr_t){(uint64_t *)PDT_ADDRESS});
	pdt_entry[1] = 0LL;
	_mem_unmap_page(pdt_entry);
}

_Bool _mem_range_is_mapped(void *start_address, uint_t length) {
	void *end_address = start_address + length;

	if(start_address < (void *)USERSPACE_VIRT_ADDRESS || start_address >= (void *)USERSPACE_VIRT_ADDRESS + NUM_PTES * PAGE_SIZE)
		return 0;
	if(end_address >= (void *)USERSPACE_VIRT_ADDRESS + NUM_PTES * PAGE_SIZE)
		return 0;

	uint64_t *page_table = _mem_map_page(_current->pagetab);
	uint64_t first_pte = ((uint64_t)start_address - USERSPACE_VIRT_ADDRESS) >> 12;
	uint64_t last_pte = ((uint64_t)end_address - USERSPACE_VIRT_ADDRESS) >> 12;
	for(uint64_t entry = first_pte; entry <= last_pte; ++entry) {
		if(!(page_table[entry] & PAGE_PRESENT)) {
			_mem_unmap_page(page_table);
			return 0;
		}
	}
	_mem_unmap_page(page_table);

	return 1;
}

void _mem_kill_overflowing_process(void) {
	c_printf( "Killing misbehaving process...\n\n" );
	c_printf( "You've got to reel in your stack space requirements, man!\n" );
	_terminate();
}

void _mem_init(void) {
	__install_isr(INT_VEC_PAGE_FAULT, __page_fault_handler);
}
