#
# SCCS ID: %W%	%G%
#
# Makefile to control the compiling, assembling and linking of
# standalone programs in the DSL.
#

#
# User supplied files
#
U_C_SRC = c_io_fake.c \
	ulibc.c user.c
U_C_OBJ = c_io_fake.o \
	ulibc.o user.o
U_S_SRC = user_start.S ulibs.S
U_S_OBJ = user_start.o ulibs.o
U_H_SRC = clock.h common.h defs.h process.h queue.h \
	scheduler.h sio.h stack.h syscall.h system.h types.h ulib.h user.h
K_C_SRC = c_io.c support.c clock.c klibc.c process.c memory.c queue.c scheduler.c sio.c \
	stack.c syscall.c system.c
K_C_OBJ = c_io.o support.o clock.o klibc.o process.o memory.o queue.o scheduler.o sio.o \
	stack.o syscall.o system.o
K_S_SRC = startup.S isr_stubs.S klibs.S
K_S_OBJ = startup.o isr_stubs.o klibs.o
K_H_SRC = clock.h common.h defs.h klib.h process.h queue.h \
	scheduler.h sio.h stack.h syscall.h system.h types.h


U_LIBS	=

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#
USER_OPTIONS = -DDUMP_QUEUES -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG

#
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
#
# Compilation/assembly control
#

#
# We only want to include from the current directory and ~wrc/include
#
INCLUDES = -I. -I/home/fac/wrc/include

#
# Compilation/assembly/linking commands and options
#
CPP = cpp
# CPPFLAGS = $(USER_OPTIONS) -nostdinc -I- $(INCLUDES)
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES)

CC = gcc
CFLAGS = -std=c99 -m64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -fno-stack-protector -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS = --64

LD = ld
LDFLAGS_KERN = -z max-page-size=0x1000

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
# use of #include/#define/#ifdef statements. However, the line numbers of
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
#

.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

.s.b:
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# Binary/source file for system bootstrap code

BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S

# Collections of files

U_OBJECTS = $(U_S_OBJ) $(U_C_OBJ)
K_OBJECTS = $(K_S_OBJ) $(K_C_OBJ)

SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

usb.image: bootstrap.b kern.b userspace.b kern.nl BuildImage #kern.dis 
	./BuildImage -d usb -o usb.image -b bootstrap.b kern.b 0x20000 userspace.b 0x60000

floppy.image: bootstrap.b kern.b kern.nl BuildImage #kern.dis 
	./BuildImage -d floppy -o floppy.image -b bootstrap.b kern.b 0x20000

kern.o: $(K_OBJECTS)
	$(LD) $(LDFLAGS_KERN) -o kern.o -Ttext 0x20000 $(K_OBJECTS) $(U_LIBS)

hd.img: usb.image
	- [ ! -e hd.img ] && bximage -q -hd -mode=flat -size=10 hd.img
	dd if=usb.image of=hd.img conv=notrunc

kern.b:	kern.o
	$(LD) $(LDFLAGS_KERN) -o kern.b -s --oformat binary -Ttext 0x20000 kern.o

userspace.o: $(U_OBJECTS)
	$(LD) $(LDFLAGS_KERN) -o userspace.o -Ttext 0x60000 $(U_OBJECTS) $(U_LIBS)

userspace.b:	userspace.o
	$(LD) $(LDFLAGS_KERN) -o userspace.b -s --oformat binary -Ttext 0x60000 userspace.o

#
# Targets for copying bootable image onto boot devices
#

floppy:	floppy.image
	dd if=floppy.image of=/dev/fd0

usb:	usb.image
	dd if=usb.image of=/local/devices/disk

#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c

Offsets:	Offsets.c
	$(CC) $(INCLUDES) -o Offsets Offsets.c

#
# Clean out this directory
#

clean:
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets

#
# Create a printable namelist from the kern.o file
#

kern.nl: kern.o
	# nm -Bng kern.o | pr -w80 -3 > kern.nl
	nm -Bn kern.o | pr -w80 -3 > kern.nl

#
# Generate a disassembly
#

kern.dis: kern.o
	dis kern.o > kern.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

bootstrap.o: bootstrap.h x86arch.h
