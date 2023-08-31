.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

USER_OBJECTS=\
	user/core.o\
	user/itoa.o\
	user/version.o \
	user/help.o