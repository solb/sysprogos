/*
** SCCS ID:	@(#)isr_stubs.S	1.7	4/9/14
**
** File:	isr_stubs.S
**
** Author:	K. Reek
**
** Contributor:	Jon Coles, Warren R. Carithers, Margaret Reek, and
**		numerous 4003-506 classes.
**
** Description:	Stubs for ISRs.
**
**	This module provides the stubs needed for interrupts to save
**	the machine state before calling the ISR.  All interrupts have
**	their own stub which pushes the interrupt number on the stack.
**	This makes it possible for a common ISR to determine which
**	interrupted occurred.
*/

#include "bootstrap.h"

/*
** Configuration options - define in Makefile
**
**	ISR_DEBUGGING_CODE	include context restore debugging code
*/

	.text

/*
** Macros for the isr stubs.  Some interrupts push an error code on
** the stack and others don't; for those that don't we simply push
** a zero so that cleaning up from either type is identical.
*/

#define	ISR(vector)			\
	.globl	__isr_##vector		; \
__isr_##vector:				; \
	pushq	$0			; \
	pushq	$vector			; \
	jmp	isr_save		;

#define	ERR_ISR(vector)		\
	.globl	__isr_##vector		; \
__isr_##vector:				; \
	pushq	$vector			; \
	jmp	isr_save		;

	.globl	__isr_table
	.globl	__isr_restore

/*
** This routine saves the machine state, calls the ISR, and then
** restores the machine state and returns from the interrupt.
**
********************************************************************
********************************************************************
** NOTE:  this code is highly application-specific, and will most **
** probably require modification to tailor it.                    **
**                                                                **
** Examples of mods:  switch to/from user stack, context switch   **
** changes, etc.                                                  **
********************************************************************
********************************************************************
*/

isr_save:
/*
** Begin by saving the state
**
** At this point, the stack looks like this:
**
**  esp ->  vector #		saved by the entry macro
**	    error code, or 0	saved by the hardware, or the entry macro
**	    saved RIP		saved by the hardware
**	    saved CS		saved by the hardware
**	    saved RFLAGS	saved by the hardware
*/
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%r11
	pushq	%r10
	pushq	%r9
	pushq	%r8
	pushq	%rax
	pushq	%rcx
	pushq	%rdx
	pushq	%rbx
	pushq	%rbp
	pushq	%rsi
	pushq	%rdi

	xorq	%rdx, %rdx
	movw	%ds, %dx
	pushq	%rdx
	movw	%es, %dx
	pushq	%rdx
	movw	%fs, %dx
	pushq	%rdx
	movw	%gs, %dx
	pushq	%rdx
	

/*
** Stack contents (all 64-bit longwords) and offsets from ESP:
**   GS FS ES DS EDI ESI EBP EBX EDX ECX EAX
**   0  8  16 24 32  40  48  56  64  72  80
**
**   R8 R9 R10 R11 R12 R13 R14 R15 vec cod RIP CS  RFL RSP SS
**   88 96 104 112 120 128 136 144 152 160 168 176 184 192 200
**   
** Set up parameters for the ISR call.
*/
	movq	152(%rsp),%rdi	// get vector number and error code
	movq	160(%rsp),%rsi

/*
** MOD for 20135 CSCI452
*/

	.globl	_current
	.globl	_system_esp

	movq	_current, %rdx	// save context pointer for the current proc
	movq	%rsp, (%rdx)	// (must be the first PCB field)

		// note:  this is inherently non-reeentrant!
	movq	_system_esp, %rsp	// switch to OS stack

/*
** END MOD for 20135 CSCI452
*/

/*
** Call the ISR
*/
	movq	__isr_table(,%rdi,8),%rbx
	call	*%rbx

/*
** Context restore begins here
*/

__isr_restore:

/*
** MOD for 20135 CSCI452
*/

	movq	_current, %rbx	// return to user stack
	movq	(%rbx), %rsp	// ESP now points to context save area

/*
** END MOD for 20135 CSCI452
*/

#ifdef ISR_DEBUGGING_CODE
/*
** DEBUGGING CODE PART 1
**
** This code will execute during each context restore, and 
** should be modified to print out whatever debugging information
** is desired.
**
** By default, it prints out the CPU context being restored; it
** relies on the standard save sequence (see above).
*/

/*
** MOD for 20135 CSCI452
*/


	.globl  _system_time

	movq	$0, %rdi
	movq	$1, %rsi
	movq	$fmt, %rdx
	movq	%rsp, %rcx		// get the context pointer
	/*
	** In addition to the basic context information, print
	** the current system time and the PID and PPID of the
	** process whose context is being restored.
	**
	** THE CONSTANT IN THIS movl INSTRUCTION MUST BE CHANGED
	** IF THE LOCATION OF EITHER FIELD IN THE PCB CHANGES!
	*/
	movq    _system_time, %r8    // and the current system time
	movl    20(%rbx), %eax  // get PID and PPID of this process
	movq	%rax, %r9
	call	c_printf_at

/*
** END MOD for 20135 CSCI452
*/

/*
** END OF DEBUGGING CODE PART 1
*/
#endif

/*
** Restore the context.
*/
	popq	%gs
	popq	%fs
	popq	%rax
	movw	%ax, %es
	popq	%rax
	movw	%ax, %ds
	popq	%rdi
	popq	%rsi
	popq	%rbp
	popq	%rbx
	popq	%rdx
	popq	%rcx
	popq	%rax
	popq	%r8
	popq	%r9
	popq	%r10
	popq	%r11
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	addq	$16, %rsp	// discard the error code and vector
	iretq			// and return

#ifdef ISR_DEBUGGING_CODE
/*
** DEBUGGING CODE PART 2
*/
/*
** MOD for 20135 CSCI452
*/
fmt:	.ascii	"context %08x time %08x ppid/pid %08x\n"
	.ascii	" ss=%08x  gs=%08x  fs=%08x  es=%08x  ds=%08x\n"
/*
** END MOD for 20135 CSCI452
*/
	.ascii	"edi=%08x esi=%08x ebp=%08x esp=%08x ebx=%08x\n"
	.ascii	"edx=%08x ecx=%08x eax=%08x vec=%08x cod=%08x\n"
	.string	"eip=%08x  cs=%08x efl=%08x\n"

/*
** END OF DEBUGGING CODE PART 2
*/
#endif

/*
** Here we generate the individual stubs for each interrupt.
*/
ISR(0x00);	ISR(0x01);	ISR(0x02);	ISR(0x03);
ISR(0x04);	ISR(0x05);	ISR(0x06);	ISR(0x07);
ERR_ISR(0x08);	ISR(0x09);	ERR_ISR(0x0a);	ERR_ISR(0x0b);
ERR_ISR(0x0c);	ERR_ISR(0x0d);	ERR_ISR(0x0e);	ISR(0x0f);
ISR(0x10);	ERR_ISR(0x11);	ISR(0x12);	ISR(0x13);
ISR(0x14);	ISR(0x15);	ISR(0x16);	ISR(0x17);
ISR(0x18);	ISR(0x19);	ISR(0x1a);	ISR(0x1b);
ISR(0x1c);	ISR(0x1d);	ISR(0x1e);	ISR(0x1f);
ISR(0x20);	ISR(0x21);	ISR(0x22);	ISR(0x23);
ISR(0x24);	ISR(0x25);	ISR(0x26);	ISR(0x27);
ISR(0x28);	ISR(0x29);	ISR(0x2a);	ISR(0x2b);
ISR(0x2c);	ISR(0x2d);	ISR(0x2e);	ISR(0x2f);
ISR(0x30);	ISR(0x31);	ISR(0x32);	ISR(0x33);
ISR(0x34);	ISR(0x35);	ISR(0x36);	ISR(0x37);
ISR(0x38);	ISR(0x39);	ISR(0x3a);	ISR(0x3b);
ISR(0x3c);	ISR(0x3d);	ISR(0x3e);	ISR(0x3f);
ISR(0x40);	ISR(0x41);	ISR(0x42);	ISR(0x43);
ISR(0x44);	ISR(0x45);	ISR(0x46);	ISR(0x47);
ISR(0x48);	ISR(0x49);	ISR(0x4a);	ISR(0x4b);
ISR(0x4c);	ISR(0x4d);	ISR(0x4e);	ISR(0x4f);
ISR(0x50);	ISR(0x51);	ISR(0x52);	ISR(0x53);
ISR(0x54);	ISR(0x55);	ISR(0x56);	ISR(0x57);
ISR(0x58);	ISR(0x59);	ISR(0x5a);	ISR(0x5b);
ISR(0x5c);	ISR(0x5d);	ISR(0x5e);	ISR(0x5f);
ISR(0x60);	ISR(0x61);	ISR(0x62);	ISR(0x63);
ISR(0x64);	ISR(0x65);	ISR(0x66);	ISR(0x67);
ISR(0x68);	ISR(0x69);	ISR(0x6a);	ISR(0x6b);
ISR(0x6c);	ISR(0x6d);	ISR(0x6e);	ISR(0x6f);
ISR(0x70);	ISR(0x71);	ISR(0x72);	ISR(0x73);
ISR(0x74);	ISR(0x75);	ISR(0x76);	ISR(0x77);
ISR(0x78);	ISR(0x79);	ISR(0x7a);	ISR(0x7b);
ISR(0x7c);	ISR(0x7d);	ISR(0x7e);	ISR(0x7f);
ISR(0x80);	ISR(0x81);	ISR(0x82);	ISR(0x83);
ISR(0x84);	ISR(0x85);	ISR(0x86);	ISR(0x87);
ISR(0x88);	ISR(0x89);	ISR(0x8a);	ISR(0x8b);
ISR(0x8c);	ISR(0x8d);	ISR(0x8e);	ISR(0x8f);
ISR(0x90);	ISR(0x91);	ISR(0x92);	ISR(0x93);
ISR(0x94);	ISR(0x95);	ISR(0x96);	ISR(0x97);
ISR(0x98);	ISR(0x99);	ISR(0x9a);	ISR(0x9b);
ISR(0x9c);	ISR(0x9d);	ISR(0x9e);	ISR(0x9f);
ISR(0xa0);	ISR(0xa1);	ISR(0xa2);	ISR(0xa3);
ISR(0xa4);	ISR(0xa5);	ISR(0xa6);	ISR(0xa7);
ISR(0xa8);	ISR(0xa9);	ISR(0xaa);	ISR(0xab);
ISR(0xac);	ISR(0xad);	ISR(0xae);	ISR(0xaf);
ISR(0xb0);	ISR(0xb1);	ISR(0xb2);	ISR(0xb3);
ISR(0xb4);	ISR(0xb5);	ISR(0xb6);	ISR(0xb7);
ISR(0xb8);	ISR(0xb9);	ISR(0xba);	ISR(0xbb);
ISR(0xbc);	ISR(0xbd);	ISR(0xbe);	ISR(0xbf);
ISR(0xc0);	ISR(0xc1);	ISR(0xc2);	ISR(0xc3);
ISR(0xc4);	ISR(0xc5);	ISR(0xc6);	ISR(0xc7);
ISR(0xc8);	ISR(0xc9);	ISR(0xca);	ISR(0xcb);
ISR(0xcc);	ISR(0xcd);	ISR(0xce);	ISR(0xcf);
ISR(0xd0);	ISR(0xd1);	ISR(0xd2);	ISR(0xd3);
ISR(0xd4);	ISR(0xd5);	ISR(0xd6);	ISR(0xd7);
ISR(0xd8);	ISR(0xd9);	ISR(0xda);	ISR(0xdb);
ISR(0xdc);	ISR(0xdd);	ISR(0xde);	ISR(0xdf);
ISR(0xe0);	ISR(0xe1);	ISR(0xe2);	ISR(0xe3);
ISR(0xe4);	ISR(0xe5);	ISR(0xe6);	ISR(0xe7);
ISR(0xe8);	ISR(0xe9);	ISR(0xea);	ISR(0xeb);
ISR(0xec);	ISR(0xed);	ISR(0xee);	ISR(0xef);
ISR(0xf0);	ISR(0xf1);	ISR(0xf2);	ISR(0xf3);
ISR(0xf4);	ISR(0xf5);	ISR(0xf6);	ISR(0xf7);
ISR(0xf8);	ISR(0xf9);	ISR(0xfa);	ISR(0xfb);
ISR(0xfc);	ISR(0xfd);	ISR(0xfe);	ISR(0xff);

	.data

/*
** This table contains the addresses where each of the preceding
** stubs begins.  This information is needed to initialize the
** Interrupt Descriptor Table in support.c
*/
	.globl	__isr_stub_table
__isr_stub_table:
	.quad	__isr_0x00, __isr_0x01, __isr_0x02, __isr_0x03
	.quad	__isr_0x04, __isr_0x05, __isr_0x06, __isr_0x07
	.quad	__isr_0x08, __isr_0x09, __isr_0x0a, __isr_0x0b
	.quad	__isr_0x0c, __isr_0x0d, __isr_0x0e, __isr_0x0f
	.quad	__isr_0x10, __isr_0x11, __isr_0x12, __isr_0x13
	.quad	__isr_0x14, __isr_0x15, __isr_0x16, __isr_0x17
	.quad	__isr_0x18, __isr_0x19, __isr_0x1a, __isr_0x1b
	.quad	__isr_0x1c, __isr_0x1d, __isr_0x1e, __isr_0x1f
	.quad	__isr_0x20, __isr_0x21, __isr_0x22, __isr_0x23
	.quad	__isr_0x24, __isr_0x25, __isr_0x26, __isr_0x27
	.quad	__isr_0x28, __isr_0x29, __isr_0x2a, __isr_0x2b
	.quad	__isr_0x2c, __isr_0x2d, __isr_0x2e, __isr_0x2f
	.quad	__isr_0x30, __isr_0x31, __isr_0x32, __isr_0x33
	.quad	__isr_0x34, __isr_0x35, __isr_0x36, __isr_0x37
	.quad	__isr_0x38, __isr_0x39, __isr_0x3a, __isr_0x3b
	.quad	__isr_0x3c, __isr_0x3d, __isr_0x3e, __isr_0x3f
	.quad	__isr_0x40, __isr_0x41, __isr_0x42, __isr_0x43
	.quad	__isr_0x44, __isr_0x45, __isr_0x46, __isr_0x47
	.quad	__isr_0x48, __isr_0x49, __isr_0x4a, __isr_0x4b
	.quad	__isr_0x4c, __isr_0x4d, __isr_0x4e, __isr_0x4f
	.quad	__isr_0x50, __isr_0x51, __isr_0x52, __isr_0x53
	.quad	__isr_0x54, __isr_0x55, __isr_0x56, __isr_0x57
	.quad	__isr_0x58, __isr_0x59, __isr_0x5a, __isr_0x5b
	.quad	__isr_0x5c, __isr_0x5d, __isr_0x5e, __isr_0x5f
	.quad	__isr_0x60, __isr_0x61, __isr_0x62, __isr_0x63
	.quad	__isr_0x64, __isr_0x65, __isr_0x66, __isr_0x67
	.quad	__isr_0x68, __isr_0x69, __isr_0x6a, __isr_0x6b
	.quad	__isr_0x6c, __isr_0x6d, __isr_0x6e, __isr_0x6f
	.quad	__isr_0x70, __isr_0x71, __isr_0x72, __isr_0x73
	.quad	__isr_0x74, __isr_0x75, __isr_0x76, __isr_0x77
	.quad	__isr_0x78, __isr_0x79, __isr_0x7a, __isr_0x7b
	.quad	__isr_0x7c, __isr_0x7d, __isr_0x7e, __isr_0x7f
	.quad	__isr_0x80, __isr_0x81, __isr_0x82, __isr_0x83
	.quad	__isr_0x84, __isr_0x85, __isr_0x86, __isr_0x87
	.quad	__isr_0x88, __isr_0x89, __isr_0x8a, __isr_0x8b
	.quad	__isr_0x8c, __isr_0x8d, __isr_0x8e, __isr_0x8f
	.quad	__isr_0x90, __isr_0x91, __isr_0x92, __isr_0x93
	.quad	__isr_0x94, __isr_0x95, __isr_0x96, __isr_0x97
	.quad	__isr_0x98, __isr_0x99, __isr_0x9a, __isr_0x9b
	.quad	__isr_0x9c, __isr_0x9d, __isr_0x9e, __isr_0x9f
	.quad	__isr_0xa0, __isr_0xa1, __isr_0xa2, __isr_0xa3
	.quad	__isr_0xa4, __isr_0xa5, __isr_0xa6, __isr_0xa7
	.quad	__isr_0xa8, __isr_0xa9, __isr_0xaa, __isr_0xab
	.quad	__isr_0xac, __isr_0xad, __isr_0xae, __isr_0xaf
	.quad	__isr_0xb0, __isr_0xb1, __isr_0xb2, __isr_0xb3
	.quad	__isr_0xb4, __isr_0xb5, __isr_0xb6, __isr_0xb7
	.quad	__isr_0xb8, __isr_0xb9, __isr_0xba, __isr_0xbb
	.quad	__isr_0xbc, __isr_0xbd, __isr_0xbe, __isr_0xbf
	.quad	__isr_0xc0, __isr_0xc1, __isr_0xc2, __isr_0xc3
	.quad	__isr_0xc4, __isr_0xc5, __isr_0xc6, __isr_0xc7
	.quad	__isr_0xc8, __isr_0xc9, __isr_0xca, __isr_0xcb
	.quad	__isr_0xcc, __isr_0xcd, __isr_0xce, __isr_0xcf
	.quad	__isr_0xd0, __isr_0xd1, __isr_0xd2, __isr_0xd3
	.quad	__isr_0xd4, __isr_0xd5, __isr_0xd6, __isr_0xd7
	.quad	__isr_0xd8, __isr_0xd9, __isr_0xda, __isr_0xdb
	.quad	__isr_0xdc, __isr_0xdd, __isr_0xde, __isr_0xdf
	.quad	__isr_0xe0, __isr_0xe1, __isr_0xe2, __isr_0xe3
	.quad	__isr_0xe4, __isr_0xe5, __isr_0xe6, __isr_0xe7
	.quad	__isr_0xe8, __isr_0xe9, __isr_0xea, __isr_0xeb
	.quad	__isr_0xec, __isr_0xed, __isr_0xee, __isr_0xef
	.quad	__isr_0xf0, __isr_0xf1, __isr_0xf2, __isr_0xf3
	.quad	__isr_0xf4, __isr_0xf5, __isr_0xf6, __isr_0xf7
	.quad	__isr_0xf8, __isr_0xf9, __isr_0xfa, __isr_0xfb
	.quad	__isr_0xfc, __isr_0xfd, __isr_0xfe, __isr_0xff
