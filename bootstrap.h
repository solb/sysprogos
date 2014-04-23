/*
** SCCS ID:	@(#)bootstrap.h	1.4	10/18/09
**
** File:	bootstrap.h
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers, Garrett C. Smith
**
** Description:	Addresses where various stuff goes in memory.
*/

#ifndef	_BOOTSTRAP_H
#define	_BOOTSTRAP_H

/*
** The target program itself
*/
#define TARGET_SEGMENT	0x00002000	/* 0001:0000 */
#define TARGET_ADDRESS	0x00020000	/* and upward */
#define	TARGET_STACK	0x00020000	/* and downward */

/*
** The Global Descriptor Table (0000:0500 - 0000:2500)
*/
#define	GDT_SEGMENT	0x00000050
#define	GDT_ADDRESS	0x00000500

	/* segment register values */
#define	GDT_LINEAR	0x0008		/* All of memory, R/W */
#define	GDT_CODE	0x0010		/* All of memory, R/E */
#define	GDT_DATA	0x0018		/* All of memory, R/W */
#define	GDT_STACK	0x0020		/* All of memory, R/W */
#define	GDT_CODE64	0x0028		/* All of memory, R/E */
#define GDT_TSS		0x0030		/* Location of the TSS structure, E (TWICE AS LONG!!)*/
#define GDT_USREXEC	(0x0040 | 03)
#define GDT_USRNOEX	(0x0048 | 03)

/*
** The Interrupt Descriptor Table (0000:2500 - 0000:2D00)
*/
#define	IDT_SEGMENT	0x00000250
#define IDT_ADDRESS	0x00002500

/*
** The Task State Segment
*/
#define TSS_SEGMENT 0x00000350
#define TSS_ADDRESS	0x00003500
#define TSS_SIZE	104
#define TSS_ESP0	1

/*
** The Page Tables
*/
#define PML4_SEGMENT	0x1000
#define PML4_ADDRESS	0x10000
#define	PDPT_ADDRESS	0x11000
#define PDT_ADDRESS		0x12000
#define PT_ADDRESS		0x13000
#define PAGE_LENS		0x1000	// 0x4000 words, moving by DWORD
#define PAGE_PRESENT	0x1
#define PAGE_RW			0x2
#define PAGE_USER		0x4

/*
** Physical Memory Map Table (0000:2D00 - 0000:2D08)
*/
#define	MMAP_SEGMENT	0x000002D0
#define	MMAP_ADDRESS	0x00002D00
#define	MMAP_EXT_LO	0x00	/* extended memory - low register */
#define	MMAP_EXT_HI	0x02	/* extended memory - high register */
#define	MMAP_CFG_LO	0x04	/* configured memory - low register */
#define	MMAP_CFG_HI	0x06	/* configured memory - high register */
#define	MMAP_PROGRAMS	0x08	/* # of programs loaded from disk (+ kernel) */
#define	MMAP_SECTORS	0x0a	/* table of sector counts for each program */

/*
** Real Mode Program(s) Text Area (0000:3000 - 0x7c00)
*/

#define	RMTEXT_SEGMENT	0x00000300
#define	RMTEXT_ADDRESS	0x00003000

#endif
