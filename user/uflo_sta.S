/*
** Author:	Dan Johnson
**
** Description: Stack protection-checking user process 2
*/

/*
** User UNDERFLOWSTACK attempts to underflow the stack.
*/

.globl c_puts
.globl exit

#define VIRTUAL_STACK_PAGE_ADDR 0x3ff000

aboutto:
.asciz "About to underflow the stack...\n"

bummer:
.asciz "ERROR: Successfully underflowed the stack!\n"

.globl main
main:
	// do the bad shit now...
	movq	$aboutto, %rdi
	call	c_puts
	movq	$0x400008, %rsp
	movq	$97, %rcx
	movq	$2, %rax
	int	$0x80

	// Hopefully we never make it here!
	movq	$0x400000, %rsp
	movq	$bummer, %rdi
	call	c_puts
	call	exit
