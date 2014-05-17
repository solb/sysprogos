/*
** Mount the filesystem that will house some userspace shit.
** Must be setuid root...
** Pass it some argument (e.g. unmount) to undo its dirty work.
**
** Author: Sol Boucher
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char *argv[]) {
	if(argc != 3) {
		printf("USAGE: %s <mount|unmount> <path to image file>\n", argv[0]);
		return 1;
	}

	if(!strcmp(argv[1], "mount")) {
		char opts[20];
		sprintf(opts, "loop,uid=%d", getuid());

		setuid(geteuid());
		execl("/bin/mount", "mount", "-t", "vfat", "-o", opts, argv[2], "/tmp/userspacemnt", NULL);
	}
	else if(!strcmp(argv[1], "unmount")) {
		setuid(geteuid());
		execl("/bin/umount", "umount", "/tmp/userspacemnt", NULL);
	}
	else {
		printf("Unrecognized directive: %s\n", argv[1]);
		return 2;
	}
	perror("loopmount_userspace");

	return 0;
}
