/*
** Author:	Sol Boucher
**
** Description: R userspace process
*/

#include "userspace.h"

/*
** User R loops 3 times reading/writing, then exits.
*/

int main( void ) {
	c_puts( "Shell running.\n" );
	int running = 1;
	while(running) {
		writech('\n');
		writech('$');
		char cmd[12];
		char ch;
		int idx = 0;
		cmd[idx++] = '/';
		while((ch = readch()) != '\n') {
			if (ch == 8) {
				if (idx > 1) {
					writech(ch);
					writech(' ');
					writech(ch);
					cmd[idx--] = '\0';
				}
			} else {
				if (idx < 9) {
					writech(ch);
					cmd[idx++] = ch;
				}
			}
		}
		writech('\n');
		cmd[idx++] = '.';
		cmd[idx++] = 'B';
		cmd[idx++] = '\0';
		syncspawn(cmd);
	}

	c_puts( "Shell exiting\n" );
}
