
X11 = 1
X_H = -I/usr/X11R6/include -I/usr/include
XF86VMODE = 1
XF86VMODE_LIB = -lXxf86vm
XIN = 1
XINERAMA_LIB = -lXinerama
X_CPP = $(X_CPPFILES)
X11_LIB_PATH = -L/usr/X11R6/lib$(BARCH) -L/usr/lib$(BARCH) -L$(libdir)
X11_LIBS=$(X11_LIB_PATH) -lX11 $(XF86VMODE_LIB) -lXpm -lXext $(XINERAMA_LIB)
CXX = c++
CC = cc
RM = rm -vf
CFLAGS=-Wall -g $(INCL)

COLLECT = ar cru
RANLIB = ranlib

LIBNAME = libXNVCtrl.a
OBJECTS = NVCtrl.o


all:
	echo Compiling NVCtrl.c ...
	$(CC) $(CFLAGS) -I. NVCtrl.c -c -o NVCtrl.o $(X_H)
	echo Linking $(LIBNAME) ...
	$(COLLECT) $(LIBNAME) $(OBJECTS)
	$(RANLIB) $(LIBNAME)

clean:
	$(RM) $(LIBNAME) $(OBJECTS)
