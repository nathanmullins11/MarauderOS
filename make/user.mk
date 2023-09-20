.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

USER_OBJECTS=\
	user/core.o\
	user/itoa.o\
	user/version.o\
	user/time.o \
	user/help.o \
	user/doubly_linked_list.o \
	user/pcb_user_commands.o \
	user/process_queue.o