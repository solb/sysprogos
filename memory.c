// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

/*
** Name:	__page_fault_handler
**
** Description: Handles page faults
*/

#define __SP_KERNEL__

#include "memory.h"
#include "common.h"

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
** _mem_init()
**
** Initialize the memory protection module
*/
void _mem_init(void) {
	__install_isr(INT_VEC_PAGE_FAULT, __page_fault_handler);
	__expose_user_pages();
}
