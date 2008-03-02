CXX = c++
CC = cc
RM = rm -vf
OPTS=-Wall -g -fPIC #-O2

COLLECT = ar cru
RANLIB = ranlib

LIBNAME = libXNVCtrl.a
OBJECTS = NVCtrl.o


all:
	echo Compiling NVCtrl.c ...
	$(CC) $(OPTS) -I. NVCtrl.c -c -o NVCtrl.o
	echo Linking $(LIBNAME) ...
	$(COLLECT) $(LIBNAME) $(OBJECTS)
	$(RANLIB) $(LIBNAME)

clean:
	$(RM) $(LIBNAME) $(OBJECTS)
