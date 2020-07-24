Dannah Gersh
CoE 1550 - Introduction to Operating Systems
Project 2
10/16/2016

Files Included
	prodcons.c
	prodcons.h
	syscalls.h
	sys.c
	syscall_table.S
	unistd.h

Build Instructions
	copy files over to respective directories in linux-2.6.23.1 folder:
		cp unistd.h linux-2.6.23.1/include/asm-i386/unistd.h
		cp prodcons.h linux-2.6.23.1/include/linux/prodcons.h
		cp sys.c linux-2.6.23.1/kernel/sys.c
		cp syscall_table.S linux-2.6.23.1/arch/i386/kernel/syscall_table.S
		cp syscalls.h linux-2.6.23.1/include/linux/syscalls.h
		
		copy prodcons.c to folder that contains the linux-2.6.23.1 folder.
	
	build linux kernel
		make ARCH=i386 bzImage


	copy bzImage and System.map over to QEMU

	replace devel files:
		cp bzImage /boot/bzImage-devel
		cp System.map /boot/System.map-devel
	
	reboot:
		lilo
		reboot

	compile prodcons.c in thoth
		gcc -m32 -o prodcons -I /u/OSLab/USERNAME/linux-2.6.23.1/include/ prodcons.c
		
	copy prodcons executable over to QEMU
	
	run prodcons
		./prodcons [num producers] [num consumers] [buf size]