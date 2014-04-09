/*
** SCCS ID:	@(#)main.c	1.3	03/15/05
**
** File:	main.c
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers
**
** Description:	Dummy main program
*/
#include "c_io.h"
#include "support.h"

void fake_syscall(int vector, int code) {
	c_puts("Fake syscall called.\n");
}

void test_int_handler(int, int);
int test_int_caller(void);

int main(void) {
	c_puts("Starting to check PTs\n");
	for(int *spot = (int *)0x10000; spot < (int *)0x14000; ++spot) {
		if(*spot) {
			c_puts("SHIT!\n");
			break;
		}
	}
	c_puts("Verified all PTs\n");

	/*c_puts("Installing isr 80\n");
	__install_isr(80, fake_syscall);
	c_puts("Installed ISR. Invoking\n");
	__asm__("int $80");*/

	__install_isr(80, test_int_handler);
	if(test_int_caller())
		c_puts("Nothing was clobbered :)\n");
	else
		c_puts("A dark day for Middle Earth B[\n");

	c_puts("Hello, world!\n");
	return(0);
}
