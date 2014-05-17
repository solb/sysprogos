/*
** Author:	Sol Boucher
**
** Description: Stack protection-checking user process
*/

/*
** User OVERFLOWFLOWSTACK attempts to overflow the stack.
*/

.globl c_puts
.globl writech

#define VIRTUAL_STACK_PAGE_ADDR 0x3ff000

pontificating:
.asciz "Leaving a tiny amount of stack space...\n"

aboutto:
.asciz "About to overflow the stack...\n"

bummer:
.asciz "ERROR: Successfully overflowed the stack!\n"

.globl main
main:
	// Backup our stack pointer, since we're going to destroy it
	movq	%rsp, %rbx

	// Don't *quite* overflow it yet...
	movq	$pontificating, %rdi
	call	c_puts
	movq	$(VIRTUAL_STACK_PAGE_ADDR + 224), %rsp
	movq	$97, %rdi
	call	writech

	// Actually do the bad shit now...
	movq	%rbx, %rsp
	movq	$aboutto, %rdi
	call	c_puts
	movq	$(VIRTUAL_STACK_PAGE_ADDR + 216), %rsp
	movq	$98, %rdi
	call	writech

	// Hopefully we never make it here!
	movq	%rbx, %rsp
	movq	$bummer, %rdi
	call	c_puts

	// Restore the stack pointer before attempting to return
	movq	%rbx, %rsp
	ret
