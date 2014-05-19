/*
** SCCS ID:	@(#)ulibc.c	1.1	4/9/14
**
** File:	ulibc.c
**
** Author:	CSCI-452 class of 20135
**
** Contributor: Dan Johnson
** Contributor: Sol Boucher
**
** Description:	C implementations of user-level library functions
*/

#include "common.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

unsigned reads(unsigned len, char *buf) {
	char ch;
	int idx = 0;
	while((ch = readch()) != '\n') {
		if (ch == 8) { // backspace
			if (idx > 0) {
				writech(ch);
				writech(' ');
				writech(ch);
				buf[--idx] = '\0';
			}
		} else {
			if (idx < len - 1) {
				writech(ch);
				buf[idx++] = ch;
			}
		}
	}
	buf[idx] = '\0';
	writech('\n');
	return idx;
}
