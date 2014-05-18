/*
** Author:	Sol Boucher
** Author:	Dan Johnson
**
** Description: An attempted attack on the PIC
*/

#include "x86arch.h"

/*
** User STOPPIC tries to stop the PIC from sending interrupts forever.
*/

attempt:
.asciz "Attempting to maliciously disable the PICs...\n"

ability:
.asciz "ERROR: Successfully disabled the PICs. We are through.\n"

.globl main
main:
	movq	$attempt, %rdi
	call	c_puts
	movb	$0xff, %al
	outb	%al, $PIC_SLAVE_IMR_PORT
	outb	%al, $PIC_MASTER_IMR_PORT
	movq	$ability, %rdi
	call	c_puts
	ret
