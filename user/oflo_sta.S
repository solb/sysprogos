/*
** Author:	Sol Boucher
**
** Description: Stack protection-checking user process
*/

/*
** User OVERFLOWFLOWSTACK attempts to overflow the stack.
*/

.globl c_puts

#define VIRTUAL_STACK_PAGE_ADDR 0x3ff000

aboutto:
.asciz "About to overflow the stack...\n"

.globl main
main:
	movq	$aboutto, %rdi
	call	c_puts
	movq	$(VIRTUAL_STACK_PAGE_ADDR + 216), %rsp
	call	exit
