/*
** SCCS ID:	@(#)support.c	1.9	10/15/12
**
** File:	support.c
**
** Author:	4003-506 class of 20003
**
** Contributor:	K. Reek
**
** Description:	Initialization of the global descriptor table and
**		interrupt descriptor table.  Also some support routines.
**
*/

#define __SP_KERNEL__

#include "startup.h"
#include "support.h"
#include "c_io.h"
#include "x86arch.h"
#include "bootstrap.h"
#include "syscall.h"
#include "scheduler.h"

/*
** Global variables and local data types.
*/

/*
** Name:	__isr_table
**
** Description:	This is the table that contains pointers to the
**		C-language ISR for each interrupt.  These functions are
**		called from the isr stub based on the interrupt number.
*/
void ( *__isr_table[ 256 ] )( int vector, int code );

/*
** Name:	IDT_Gate
**
** Description:	Format of an IDT entry.
*/
typedef struct	{
	unsigned short	offset_15_0;
	unsigned short	segment_selector;
	unsigned short	flags;
	unsigned short	offset_31_16;
	unsigned int	offset_63_32;
	unsigned int	must_be_zero;
} IDT_Gate;

/*
** LOCAL ROUTINES - not intended to be used outside this module.
*/

/*
** Name:	__default_unexpected_handler
**
** Arguments:	The usual ISR arguments
**
** Returns:	Nothing; it never returns (though we must declare it
**		in the usual way to avoid compilation errors).
**
** Description:	This routine catches interrupts that we do not expect
**		to ever occur.  It handles them by calling __blame_and_punish.
*/
static void __default_unexpected_handler( int vector, int code ){
	c_printf( "\nVector=0x%02x, code=%d\n", vector, code );
	__blame_and_punish();
}

/*
** Name:	__default_expected_handler
**
** Arguments:	The usual ISR arguments
**
** Returns:	The usual ISR return value
**
** Description: Default handler for interrupts we expect may occur but
**		are not handling (yet).  Just reset the PIC and return.
*/

static void __default_expected_handler( int vector, int code ){
	if( vector >= 0x20 && vector < 0x30 ){
		__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
		if( vector > 0x28 ){
			__outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
		}
	}
	else {
		/*
		** All the "expected" interrupts will be handled by the
		** code above.  If we get down here, the isr table may
		** have been corrupted.  Print message and don't return.
		*/
		__panic( "Unexpected \"expected\" interrupt!" );
	}
}

/*
** Name:	__default_mystery_handler
**
** Arguments:	The usual ISR arguments
**
** Returns:	The usual ISR return value
**
** Description: Default handler for the "mystery" interrupt that 
**		comes through vector 0x27.  This is a non-repeatable
**		interrupt whose source has not been identified.
*/
static void __default_mystery_handler( int vector, int code ){

#ifdef REPORT_MYSTERY_INTS
	c_printf( "\nMystery interrupt!\nVector=0x%02x, code=%d\n",
		  vector, code );
#endif

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}

/*
** Name:	init_pic
**
** Description:	Initialize the Programmable Interrupt Controller.
*/
static void init_pic( void ){
	/*
	** ICW1
	*/
	__outb( PIC_MASTER_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4 );
	__outb( PIC_SLAVE_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4 );

	/*
	** ICW2: master offset of 20 in the IDT, slave offset of 28
	*/
	__outb( PIC_MASTER_IMR_PORT, 0x20 );
	__outb( PIC_SLAVE_IMR_PORT, 0x28 );

	/*
	** ICW3: slave attached to line 2 of master, bit mask is 00000100
	**	 slave id is 2
	*/
	__outb( PIC_MASTER_IMR_PORT, PIC_MASTER_SLAVE_LINE );
	__outb( PIC_SLAVE_IMR_PORT, PIC_SLAVE_ID );

	/*
	** ICW4
	*/
	__outb( PIC_MASTER_IMR_PORT, PIC_86MODE );
	__outb( PIC_SLAVE_IMR_PORT, PIC_86MODE );

	/*
	** OCW1: allow interrupts on all lines
	*/
	__outb( PIC_MASTER_IMR_PORT, 0x00 );
	__outb( PIC_SLAVE_IMR_PORT, 0x00 );
}

/*
** Name:	set_idt_entry
**
** Description:	Construct an entry in the IDT
** Arguments:	The entry number (vector number), and a pointer to the
**		stub (NOT the ISR routine) that handles that interrupt.
*/
static void set_idt_entry( int entry, void ( *handler )( void ) ){
	IDT_Gate *g = (IDT_Gate *)IDT_ADDRESS + entry;

	g->offset_15_0 = (unsigned short)handler & 0xffff;
	g->segment_selector = 0x0010;
	g->flags = IDT_PRESENT | IDT_DPL_0 | IDT_INT32_GATE | IDT_IST_1;
	g->offset_31_16 = ((unsigned int)handler >> 16) & 0xffff;
	g->offset_63_32 = ((unsigned long long)handler >> 32) & 0xffffffff;
	g->must_be_zero = 0;
}

/*
** Name:	set_idt_dpl
**
** Description: Enable calls to the specified IDT from the given rings
** Arguments:	The vector number and ring number
*/
static void set_idt_dpl( int entry, unsigned short ring ){
	if(ring > 3){
		__panic( "Unexpected privilege ring" );
	}

	IDT_Gate *g = (IDT_Gate *)IDT_ADDRESS + entry;
	unsigned short tmp_flags = g->flags;
	tmp_flags &= ~IDT_DPL_MASK;
	tmp_flags |= ring << (8+5); // There are 8 bits of low-order zeroes
	g->flags = tmp_flags;
}

/*
** Name:	init_idt
**
** Description: Initialize the Interrupt Descriptor Table (IDT).  This
**		makes each of the entries in the IDT point to the isr stub
**		for that entry, and installs a default handler in the
**		handler table.  Specific handlers are then installed for
**		those interrupts we may get before a real handler is set up.
*/
static void init_idt( void ){
	int i;
	extern	void	( *__isr_stub_table[ 256 ] )( void );

	/*
	** Make each IDT entry point to the stub for that vector.
	** Also make each entry in the ISR table point to the default handler.
	*/
	for ( i=0; i < 256; i++ ){
		set_idt_entry( i, __isr_stub_table[ i ] );
		__install_isr( i, __default_unexpected_handler );
	}

	// Allow userspace syscall interrupts
	set_idt_dpl( INT_VEC_SYSCALL, 3 );

	/*
	** Install the handlers for interrupts that have a specific handler.
	*/
	__install_isr( INT_VEC_KEYBOARD, __default_expected_handler );
	__install_isr( INT_VEC_TIMER,    __default_expected_handler );
	__install_isr( INT_VEC_MYSTERY,  __default_mystery_handler );
	//Mystery USB unplug/plug interrupt that I don't care about anymore
	__install_isr( 0x2a,  __default_mystery_handler );
}

/*
** END OF LOCAL ROUTINES.
*/

/*
** Name:	__blame_and_punish
**
** Description: Checks whether the supervisor or userspace was responsible for an exception.
**		In the former case, panics the kernel; in the latter, kills the current process.
*/
void __blame_and_punish(void) {
	if( _current->context->cs & 3 ) { // Userspace code was running
		// Kill it
		c_printf( "Killing misbehaving process...\n" );
		_terminate();
	}
	else { // Kernel code is at fault
		__panic( "Unexpected interrupt generated by supervisor\n" );
	}
}

/*
** Name:	__panic
*/
void __panic( char *reason ){
	__asm( "cli" );
	c_printf( "\nPANIC: %s\nHalting...", reason );
	for(;;){
		;
	}
}

/*
** Name:	__init_interrupts
**
** Description:	(Re)initialize the interrupt system.  This includes
**		initializing the IDT and the PIC.  It is up to the
**		user to enable processor interrupts when they're ready.
*/
void __init_interrupts( void ){
	init_idt();
	init_pic();
}

/*
** Name:	__install_isr
*/
void ( *__install_isr( int vector, void ( *handler )( int vector, int code ) ) )( int vector, int code ){
	void	( *old_handler )( int vector, int code );

	old_handler = __isr_table[ vector ];
	__isr_table[ vector ] = handler;
	return old_handler;
}

/*
** Name:	__delay
*/
void __delay( int tenths ){
	int	i;

	while( --tenths >= 0 ){
		for( i = 0; i < 10000000; i += 1 )
			;
	}
}
