/*
** Author:	Dan Johnson
**
** Description: Interactive shell!
*/

#include "userspace.h"

/*
** Dan's wonderful interactive shell
*/

int main( void ) {
	c_puts( "Shell running.\n" );
	int running = 1;
	while(running) {
		writech('$');
		char cmd[12];
		cmd[0] = '/';
		int idx = reads(9, cmd + 1) + 1;
		cmd[idx++] = '.';
		cmd[idx++] = 'b';
		cmd[idx++] = '\0';
		syncspawn(cmd);
	}

	c_puts( "Shell exiting\n" );
}
