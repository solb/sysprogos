#
# SCCS ID: %W%	%G%
#
# Makefile to control the compiling, assembling and linking of
# standalone programs in the DSL.
#

#
# User supplied files
#

# Course-generated test files included with the baseline distribution. These are not usually included
#U_C_BIN = user/init.b user/user_a.b user/user_b.b user/user_c.b user/user_d.b user/user_e.b user/user_f.b user/user_g.b user/user_h.b user/user_j.b user/user_k.b user/user_l.b user/user_m.b user/user_n.b user/user_p.b user/user_q.b user/user_r.b user/user_s.b user/user_t.b user/user_u.b user/user_w.b user/user_x.b user/user_y.b user/user_z.b user/tests.b
#U_C_OBJ = user/init.o user/user_a.o user/user_b.o user/user_c.o user/user_d.o user/user_e.o user/user_f.o user/user_g.o user/user_h.o user/user_j.o user/user_k.o user/user_l.o user/user_m.o user/user_n.o user/user_p.o user/user_q.o user/user_r.o user/user_s.o user/user_t.o user/user_u.o user/user_w.o user/user_x.o user/user_y.o user/user_z.o user/tests.o

U_C_BIN = user/init.b user/shell.b user/cat.b user/dref_nul.b user/oflo_sta.b user/uflo_sta.b user/stp_pic.b
U_C_OBJ = user/init.o user/shell.o user/cat.o user/dref_nul.o user/oflo_sta.o user/uflo_sta.o user/stp_pic.o
A_C_SRC = ulibc.c c_io_user.c c_io_shared.c
A_C_OBJ = ulibc.o c_io_user.o c_io_shared.o
A_H_SRC = clock.h common.h defs.h process.h queue.h \
	scheduler.h sio.h stack.h syscall.h system.h types.h ulib.h
A_S_SRC = ulibs.S
A_S_OBJ = ulibs.o
K_C_SRC = c_io_shared.c c_io_kern.c support.c clock.c klibc.c process.c memory.c queue.c scheduler.c sio.c \
	stack.c syscall.c system.c filesys.c
K_C_OBJ = c_io_shared.o c_io_kern.o support.o clock.o klibc.o process.o memory.o queue.o scheduler.o sio.o \
	stack.o syscall.o system.o filesys.o
K_S_SRC = startup.S isr_stubs.S klibs.S
K_S_OBJ = startup.o isr_stubs.o klibs.o
K_H_SRC = clock.h common.h defs.h klib.h process.h queue.h \
	scheduler.h sio.h stack.h syscall.h system.h types.h

U_LIBS	=

.SECONDARY: $(wildcard user/*.lo)

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG

#
# Program load points
#
KERNEL_ADDRESS = 0x20000
FILESYSTEM_PHYS_ADDRESS = 0x40000
FILESYSTEM_VIRT_ADDRESS = 0x40000
USERSPACE_VIRT_ADDRESS = 0x200000

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
CPPFLAGS = $(USER_OPTIONS) -nostdinc $(INCLUDES) -DFILESYSTEM_PHYS_ADDRESS=$(FILESYSTEM_PHYS_ADDRESS) -DFILESYSTEM_VIRT_ADDRESS=$(FILESYSTEM_VIRT_ADDRESS) -DUSERSPACE_VIRT_ADDRESS=$(USERSPACE_VIRT_ADDRESS)

CC = gcc
CFLAGS = -std=c99 -m64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -fno-stack-protector -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS = --64

LD = ld
LDFLAGS_KERN = -z max-page-size=0x1000
LDFLAGS_USER = -L.

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

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $*.c

user/%.lo: user/%.o user_start.o libuser.a
	$(LD) $(LDFLAGS_USER) $(LDFLAGS_KERN) -Ttext $(USERSPACE_VIRT_ADDRESS) -o $@ user_start.o $< -luser

user/%.b: user/%.lo
	$(LD) $(LDFLAGS_KERN) -o $@ -s --oformat binary -Ttext $(USERSPACE_VIRT_ADDRESS) $<

# Binary/source file for system bootstrap code

BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S

# Collections of files

U_OBJECTS = $(U_S_OBJ) $(U_C_OBJ)
K_OBJECTS = $(K_S_OBJ) $(K_C_OBJ)

SOURCES = $(BOOT_SRC) $(U_S_SRC) $(U_C_SRC) $(K_C_SRC) $(K_S_SRC)

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

usb.image: bootstrap.b kern.b userspace.img kern.nl BuildImage
	./BuildImage -d usb -o usb.image -b bootstrap.b kern.b $(KERNEL_ADDRESS) userspace.img $(FILESYSTEM_PHYS_ADDRESS)

floppy.image: bootstrap.b kern.b kern.nl BuildImage #kern.dis 
	./BuildImage -d floppy -o floppy.image -b bootstrap.b kern.b $(KERNEL_ADDRESS)

bootstrap.b: bootstrap.s
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

kern.o: $(K_OBJECTS)
	$(LD) $(LDFLAGS_KERN) -o kern.o -Ttext $(KERNEL_ADDRESS) $(K_OBJECTS) $(U_LIBS)

libuser.a: $(A_C_OBJ) $(A_S_OBJ)
	ar rcs libuser.a $(A_C_OBJ) $(A_S_OBJ)

hd.img: usb.image
	- [ ! -e hd.img ] && bximage -q -hd -mode=flat -size=10 hd.img
	dd if=usb.image of=hd.img conv=notrunc

kern.b:	kern.o
	$(LD) $(LDFLAGS_KERN) -o kern.b -s --oformat binary -Ttext $(KERNEL_ADDRESS) kern.o

# You need to (manually) build loopmount_userspace in order for this to ever work.
%.img:
	$(RM) $@
	fallocate -l 300K $@
	mkdosfs -F 32 $@
	mkdir -p /tmp/userspacemnt
	./loopmount_userspace mount $@
	cp $^ /tmp/userspacemnt/
	./loopmount_userspace unmount -

userspace.img: $(U_C_BIN)

loopmount_userspace: loopmount_userspace.c
	gcc -std=c99 -Wall -Wextra -pedantic -o loopmount_userspace loopmount_userspace.c
	sudo chown root loopmount_userspace
	sudo chmod u+s loopmount_userspace

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
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets libuser.a user/*.o user/*.lo user/*.b userspace.img

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
	makedepend $(INCLUDES) $(SOURCES) user/*.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

bootstrap.o: bootstrap.h x86arch.h
c_io_shared.o: c_io.h startup.h support.h x86arch.h ./stdarg.h
c_io_kern.o: c_io.h startup.h support.h x86arch.h ./stdarg.h
support.o: startup.h support.h c_io.h x86arch.h bootstrap.h syscall.h
support.o: common.h defs.h types.h system.h process.h clock.h stack.h klib.h
support.o: queue.h scheduler.h
clock.o: common.h defs.h types.h c_io.h support.h system.h process.h clock.h
clock.o: stack.h klib.h x86arch.h startup.h queue.h scheduler.h sio.h
clock.o: syscall.h
klibc.o: common.h defs.h types.h c_io.h support.h system.h process.h clock.h
klibc.o: stack.h klib.h
process.o: common.h defs.h types.h c_io.h support.h system.h process.h
process.o: clock.h stack.h klib.h queue.h
memory.o: common.h defs.h types.h c_io.h support.h system.h process.h clock.h
memory.o: stack.h klib.h memory.h bootstrap.h startup.h x86arch.h scheduler.h
memory.o: queue.h
queue.o: common.h defs.h types.h c_io.h support.h system.h process.h clock.h
queue.o: stack.h klib.h queue.h
scheduler.o: common.h defs.h types.h c_io.h support.h system.h process.h
scheduler.o: clock.h stack.h klib.h scheduler.h queue.h memory.h
sio.o: common.h defs.h types.h c_io.h support.h system.h process.h clock.h
sio.o: stack.h klib.h sio.h queue.h scheduler.h startup.h ./uart.h x86arch.h
stack.o: bootstrap.h common.h defs.h types.h c_io.h support.h system.h
stack.o: process.h clock.h stack.h klib.h queue.h memory.h
syscall.o: common.h defs.h types.h c_io.h support.h system.h process.h
syscall.o: clock.h stack.h klib.h syscall.h queue.h x86arch.h scheduler.h
syscall.o: sio.h filesys.h startup.h
system.o: common.h defs.h types.h c_io.h support.h system.h process.h clock.h
system.o: stack.h klib.h bootstrap.h syscall.h queue.h x86arch.h sio.h
system.o: scheduler.h filesys.h memory.h ulib.h
filesys.o: common.h defs.h types.h c_io.h support.h system.h process.h
filesys.o: clock.h stack.h klib.h filesys.h
startup.o: bootstrap.h
isr_stubs.o: bootstrap.h process.h common.h defs.h types.h c_io.h support.h
isr_stubs.o: system.h stack.h klib.h clock.h
user/cat.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/cat.o: system.h process.h clock.h stack.h klib.h
user/dref_nul.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/dref_nul.o: system.h process.h clock.h stack.h klib.h
user/init.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/init.o: system.h process.h clock.h stack.h klib.h
user/shell.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/shell.o: system.h process.h clock.h stack.h klib.h
user/tests.o: user/tests.h user/userspace.h common.h defs.h types.h c_io.h
user/tests.o: support.h system.h process.h clock.h stack.h klib.h
user/user_a.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_a.o: system.h process.h clock.h stack.h klib.h
user/user_b.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_b.o: system.h process.h clock.h stack.h klib.h
user/user_c.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_c.o: system.h process.h clock.h stack.h klib.h
user/user_d.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_d.o: system.h process.h clock.h stack.h klib.h
user/user_e.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_e.o: system.h process.h clock.h stack.h klib.h
user/user_f.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_f.o: system.h process.h clock.h stack.h klib.h
user/user_g.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_g.o: system.h process.h clock.h stack.h klib.h
user/user_h.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_h.o: system.h process.h clock.h stack.h klib.h
user/user_j.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_j.o: system.h process.h clock.h stack.h klib.h
user/user_k.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_k.o: system.h process.h clock.h stack.h klib.h
user/user_l.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_l.o: system.h process.h clock.h stack.h klib.h
user/user_m.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_m.o: system.h process.h clock.h stack.h klib.h
user/user_n.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_n.o: system.h process.h clock.h stack.h klib.h
user/user_p.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_p.o: system.h process.h clock.h stack.h klib.h
user/user_q.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_q.o: system.h process.h clock.h stack.h klib.h
user/user_r.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_r.o: system.h process.h clock.h stack.h klib.h
user/user_s.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_s.o: system.h process.h clock.h stack.h klib.h
user/user_t.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_t.o: system.h process.h clock.h stack.h klib.h
user/user_u.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_u.o: system.h process.h clock.h stack.h klib.h
user/user_w.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_w.o: system.h process.h clock.h stack.h klib.h
user/user_x.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_x.o: system.h process.h clock.h stack.h klib.h
user/user_y.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_y.o: system.h process.h clock.h stack.h klib.h
user/user_z.o: user/userspace.h common.h defs.h types.h c_io.h support.h
user/user_z.o: system.h process.h clock.h stack.h klib.h
