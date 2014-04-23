// Author: Sol Boucher <slb1566@rit.edu>
// Memory management functions

/*
** Name:	__page_fault_handler
**
** Description: Handles page faults
*/

#define __SP_KERNEL__

#include "common.h"

#include "bootstrap.h"
#include "startup.h"
#include "x86arch.h"

/*
** PRIVATE FUNCTIONS
*/
static void __protect_system_pages(void) {
	uint64_t *pt = (uint64_t *)PT_ADDRESS;
	*pt &= ~PAGE_USER;
	__inv_tlb();
	// TODO I believe the user stack is being allocated within page 0
}

static void __page_fault_handler(int vector, int code) {
	c_printf("Page fault: code %d addr 0x%x\n", code, __get_cr2());
	_kpanic("mem", "Page fault", FAILURE);
}

/*
** PUBLIC FUNCTIONS
*/
void _mem_init(void) {
	__install_isr(INT_VEC_PAGE_FAULT, __page_fault_handler);
	__protect_system_pages();
}
