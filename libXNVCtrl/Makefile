
CXX = c++
CC = cc
RM = rm -vf
dl = -ldl
m = -lm
XF86VMODE = 1
XF86VMODE_INC =  
XF86VMODE_LIB = -lXxf86vm
XIN = 1
XINERAMA_INC =  
XINERAMA_LIB = -lXinerama
X11_LIB_PATH = -L/usr/X11R6/lib$(BARCH) -L/usr/lib$(BARCH) -L$(libdir)
X11_INCL=$(XF86VMODE_INC) $(XINERAMA_INC)
X11_LIBS=$(X11_LIB_PATH) -lX11 $(XF86VMODE_LIB) -lX11 -lXext -lXinerama -lXxf86vm $(XINERAMA_LIB)
X11 = X11
X_H = -I/usr/X11R6/include -I/usr/include   $(X11_INCL)
.SILENT:
CFLAGS= $(DEBUG) $(INCL) -pipe -Wall -g -I/opt/local/include -fPIC -DPIC

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
