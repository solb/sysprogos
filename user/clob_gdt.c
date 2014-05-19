/*
** Author:	Sol Boucher
**
** Description: Attempt to write INIT into the GDT
** 	Believe it or not, we've done this by accident.
*/

#include "userspace.h"

/*
** An attempt at clobbering the GDT.
*/

int main( void ) {
	readfile("/INIT.B", 0, 0x500, 0xffff);
	spawn(NULL);
	exit();
}
