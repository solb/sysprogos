/*
** SCCS ID:	@(#)system.c	1.1	4/9/14
**
** File:	system.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Miscellaneous OS support implementations
*/

#define	__SP_KERNEL__

#include "common.h"

#include "system.h"
#include "clock.h"
#include "process.h"
#include "bootstrap.h"
#include "syscall.h"
#include "sio.h"
#include "scheduler.h"
#include "filesys.h"
#include "memory.h"

// need the exit() prototype
#include "ulib.h"

/*
** PRIVATE DEFINITIONS
*/
#define VIRTUAL_STACK_PAGE_ADDR ((void *)0x3ff000)

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _create_process(ppid,entry)
**
** allocate and initialize a new process' data structures (PCB, filename)
**
** returns:
**      pointer to the new PCB
*/

pcb_t *_create_process( pid_t ppid, const char *path ) {
	pcb_t *new;
	physaddr_t stack;
	
	// allocate the new structures

	new = _pcb_alloc();
	if( new == NULL ) {
		return( NULL );
	}

	// clear all the fields in the PCB

	_kmemclr( (void *) new, sizeof(pcb_t) );

	file_entry_t info;
	if(_filesys_find_file(path, &info, 0) == FAILURE) {
		c_printf("_create_process: Unable to find binary '%s'\n", path);
		return NULL;
	}

	uint_t remaining_size = info.file_size;
	uint_t start_address =
			_filesys_calc_absolute_cluster_loc((info.first_cluster_hi << 16) | info.first_cluster_low);
	c_printf("found init: addr 0x%lx size 0x%lx\n", start_address, remaining_size);
	uint64_t table[PAGES_PER_USERSPACE_PROCESS];
	for(uint64_t count = 0; count < info.file_size / PAGE_SIZE; ++count) {
		physaddr_t pf = _mem_page_frame_alloc();
		table[count] = (uint64_t)pf.addr | PAGE_PRESENT | PAGE_RW | PAGE_USER;
		void *mapped = _mem_map_page(pf);
		_filesys_readfile(mapped, start_address, count << 12,
				remaining_size >= PAGE_SIZE ? PAGE_SIZE : remaining_size);
		_mem_unmap_page(mapped);
		remaining_size -= PAGE_SIZE;
	}

	new->pagetab = _mem_page_frame_alloc();
	uint64_t *table_inmem = _mem_map_page(new->pagetab);
	_kmemcpy((void *)table_inmem, (void *)table, PAGES_PER_USERSPACE_PROCESS * sizeof(uint64_t));
	stack = _mem_page_frame_alloc();
	table_inmem[NUM_PTES - 1] = (uint64_t)stack.addr | PAGE_PRESENT | PAGE_RW | PAGE_USER;
	_mem_unmap_page(table_inmem);

	/*
	** We need to set up the initial stack contents for the new
	** process.  The low end of the initial stack must look like this:
	**
	**      esp ->  ?     <- context save area
	**              ...   <- context save area
	**              ?     <- context save area
	**              exit  <- return address for faked call to main()
	**              0     <- last word in stack
	*/

	// first, create a pointer to the longword after the stack

	void *mapped = _mem_map_page( stack );
	uint64_t *ptr = (uint64_t *)( mapped + PAGE_SIZE );

	// save the buffering 0 at the end

	*--ptr = 0;

	// fake a return address so that if the user function returns
	// without calling exit(), we return "into" exit()

	//FIXME: don't know the address of the userspace exit function
	//*--ptr = (uint64_t) exit;
	*--ptr = (uint64_t) 0;

	uint64_t *top_stack = ptr;

	// locate the context save area

	context_t *context = ((context_t *) ptr) - 1;

	// fill in the non-zero entries in the context save area

	context->rip    = USERSPACE_VIRT_ADDRESS;
	context->cs     = GDT_USREXEC;
	context->ss     = GDT_USRNOEX;
	context->ds     = GDT_USRNOEX;
	context->es     = GDT_USRNOEX;
	context->fs     = GDT_USRNOEX;
	context->gs     = GDT_USRNOEX;
	context->rflags = DEFAULT_EFLAGS;
	context->rsp    = (uint64_t)(VIRTUAL_STACK_PAGE_ADDR + ((void *)top_stack - mapped));

	_mem_unmap_page( mapped );
	new->context = (context_t *)(VIRTUAL_STACK_PAGE_ADDR + ((void *)context - mapped));

	// fill in the remaining important fields

	new->pid  = _next_pid++;
	new->ppid = ppid;
	new->prio = PRIO_DEFAULT;
	
	// all done - return the new PCB

	return( new );
}

/*
** _init - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/

void _init( void ) {
	pcb_t *pcb;

	/*
	** BOILERPLATE CODE - taken from basic framework
	**
	** Initialize interrupt stuff.
	*/

	__init_interrupts();	// IDT and PIC initialization

	/*
	** Console I/O system.
	*/

	c_io_init();
	c_setscroll( 0, 7, 99, 99 );
	c_puts_at( 0, 6, "================================================================================" );

	/*
	** 20135-SPECIFIC CODE STARTS HERE
	*/

	/*
	** Initialize various OS modules
	**
	** Note:  the clock, SIO, and syscall modules also install
	** their ISRs.
	*/

	c_puts( "Module init: " );

	_mem_init();
	_que_init();		// must be first
	_pcb_init();
	_stack_init();
	_sched_init();
	_sio_init();
	_sys_init();
	_clock_init();
	_filesys_init();

	c_puts( "\n" );

	/*
	** Create the initial system ESP and populate the TSS
	**
	** The former will be the address of the next-to-last
	** longword in the system stack.
	*/

	_system_esp = ((uint32_t *) ( (&_system_stack) + 1)) - 2;
	_stack_mktss();

	/*
	** Create the initial process
	'
	**
	** Code mostly stolen from _sys_fork(); if that routine
	** changes, SO MUST THIS!!!
	*/


	pcb = _create_process( 0, "\\~1         " );
	if( pcb == NULL ) {
		_kpanic( "_init", "init() creation failed", FAILURE );
	}

	_pcb_dump( "initial", pcb );

	// make this the first process

	_schedule( pcb );
	_dispatch();

	c_puts( (char *)(USERSPACE_VIRT_ADDRESS + 0x1970) );

	/*
	** Turn on the SIO receiver (the transmitter will be turned
	** on/off as characters are being sent)
	*/

	_sio_enable( SIO_RX );

	/*
	** END OF 20135-SPECIFIC CODE
	**
	** Finally, report that we're all done.
	*/

	c_puts( "System initialization complete.\n" );
}
