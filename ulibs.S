/*
** sccs id:	@(#)ulibs.S	1.1	4/9/14
**
** File:	ulibs.S
**
** Author:	CSCI-452 class of 20135
**
** Contributor:
**
** Description:	Assembly implementations of user-level library functions
*/

#define	__SP_ASM__

#include "syscall.h"

/*
** System call stubs
**
** All have the same structure:
**
**	move parameters 1 and 2 into ECX and EDX
**      move a code into EAX
**      generate the interrupt
**      return to the caller
**
** As these are simple "leaf" routines, we don't use
** the standard enter/leave method to set up a stack
** frame - that takes time, and we don't really need it.
*/

#define SYSCALL0(name) \
	.globl	name              ; \
name:                             ; \
	movl	$SYS_##name, %eax ; \
	int	$INT_VEC_SYSCALL  ; \
	ret

#define SYSCALL1(name) \
	.globl	name              ; \
name:                             ; \
	movq	%rdi, %rcx     ; \
	movl	$SYS_##name, %eax ; \
	int	$INT_VEC_SYSCALL  ; \
	ret

#define SYSCALL2(name) \
	.globl	name              ; \
name:                             ; \
	movq	%rdi, %rcx     ; \
	movq	%rsi, %rdx     ; \
	movl	$SYS_##name, %eax ; \
	int	$INT_VEC_SYSCALL  ; \
	ret

#define SYSCALL3(name) \
	.globl	name              ; \
name:                             ; \
	movq	%rdx, %r8      ; \
	movq	%rdi, %rcx     ; \
	movq	%rsi, %rdx     ; \
	movl	$SYS_##name, %eax ; \
	int	$INT_VEC_SYSCALL  ; \
	ret
	
#define SYSCALL4(name) \
	.globl	name              ; \
name:                             ; \
	movq	%rcx, %r9      ; \
	movq	%rdx, %r8      ; \
	movq	%rdi, %rcx     ; \
	movq	%rsi, %rdx     ; \
	movl	$SYS_##name, %eax ; \
	int	$INT_VEC_SYSCALL  ; \
	ret

/*
** for debugging purposes, these could be changed to expand into
** (e.g., for writech) something like this:
**
**      .globl  writech
**      .globl  c_printf
** writech:       movl    $SYS_writech, %eax
**      int     $INT_VEC_SYSCALL
**      pushl   $write_fmt
**      call    c_printf
**      addl    $4, %esp
**      ret
** write_fmt: .string "writech() is returning to %08x after printing %08x\n"
*/

SYSCALL0(exit)
SYSCALL0(readch)
SYSCALL1(writech)
SYSCALL1(writes)
SYSCALL1(spawn)
SYSCALL1(sleep)
SYSCALL0(getprio)
SYSCALL1(setprio)
SYSCALL0(getpid)
SYSCALL0(getppid)
SYSCALL0(gettime)
SYSCALL3(c_putchar_at)
SYSCALL4(readfile)
SYSCALL3(readdir)
SYSCALL1(mkdir)

// This is a bogus system call; it's here so that we can test
// our handling of out-of-range syscall codes in the syscall ISR.

SYSCALL0(bogus)
