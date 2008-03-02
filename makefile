CC=cc
CXX=c++
MAKEDEPEND	= /usr/X11R6//bin/makedepend -Y
OPTS=-Wall -O2
COLLECT = ar cru
RANLIB = ranlib
LNK = ln -s
RM = rm -f

prefix		= /opt/local
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
datadir		= ${prefix}/share
includedir	= ${prefix}/include
libdir		= ${exec_prefix}/lib
mandir		= ${prefix}/man
srcdir		= .

TARGET  = oyranos

VERSION_A = 0
VERSION_B = 0
VERSION_C = 1
VERSION = $(VERSION_A).$(VERSION_B).$(VERSION_C)
LIBSONAMEFULL = lib$(TARGET).so.$(VERSION)
LIBSONAME = lib$(TARGET).so.$(VERSION_A)
LIBNAME = lib$(TARGET).a
LIB_MONI_SONAMEFULL = lib$(TARGET)_moni.so.$(VERSION)
LIB_MONI_SONAME = lib$(TARGET)_moni.so.$(VERSION_A)
LIB_MONI_NAME = lib$(TARGET)_moni.a

#APPLE = 1
FLU = 1
DL = --ldflags # --ldstaticflags

ifdef FLU
FLU_H = -DHAVE_FLU
endif

LINK_FLAGS = -shared -fpic -ldl
LINK_NAME = -Wl,-soname -Wl,$(LIBSONAME)
LINK_NAME_M = -Wl,-soname -Wl,$(LIB_MONI_SONAME)

CXXFLAGS=$(OPTS) $(INCL) $(FLU_H)
INCL=-I$(includedir) -I/usr/X11R6/include -I$(srcdir)
CFLAGS = $(OPTS) $(INCL)

X11_LIBS=-L/usr/X11R6/lib -lX11

FLTK_LIBS=#`fltk-config --use-images $(DL)`

KDB_LIBS=-lkdb

ifdef FLU
FLU_LIBS=#`flu-config $(DL)`
endif


LDLIBS = -L$(libdir) -L./ $(FLTK_LIBS) \
	$(KDB_LIBS) #-llcms $(FLU_LIBS)


CPP_HEADERS = \
	oyranos.h \
	oyranos_debug.h \
	oyranos_definitions.h \
	oyranos_helper.h \
	oyranos_monitor.h
#	fl_oyranos.h
CFILES = \
	oyranos.c
CFILESC = \
	oyranos_debug.c
CFILES_MONI = \
    oyranos_monitor.c
CFILES_GAMMA = \
    oyranos_gamma.c
CPPFILES =
CXXFILES =
#	fl_oyranos.cxx
DOKU = \
        AUTHORS \
        ChangeLog \
        COPYING \
        README
FLUID = #\
	fl_oyranos.fl

SOURCES = $(CPPFILES) $(CXXFILES) $(CPP_HEADERS) $(CFILES) $(CFILESC) \
		  $(CFILES_MONI) $(CFILES_GAMMA) test.c test2.cpp
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o) $(CFILES:.c=.o) $(CFILESC:.c=.o)
MONI_OBJECTS = $(CPPFILES_MONI:.cpp=.o) $(CXXFILESMONI:.cxx=.o) $(CFILES_MONI:.c=.o)

REZ     = /Developer/Tools/Rez -t APPL -o $(TARGET) /opt/local/include/FL/mac.r
ifdef APPLE
APPLE   = $(REZ)
endif

topdir  = ..
dir     = Entwickeln
timedir = $(topdir)/$(dir)
mtime   = `find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g`

.SILENT:

all:	$(TARGET) $(TARGET)_moni $(TARGET)_gamma test2

$(TARGET):	$(OBJECTS)
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME) -o $(LIBSONAMEFULL) \
	$(OBJECTS) \
	$(LDLIBS) \
	$(APPLE)
	$(RM) $(LIBSONAME) && $(LNK) $(LIBSONAMEFULL) $(LIBSONAME)

$(TARGET)_moni:	$(MONI_OBJECTS)
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME_M) $(X11_LIBS) \
	-o $(LIB_MONI_SONAMEFULL) \
	$(MONI_OBJECTS) \
	$(APPLE)
	$(RM) $(LIB_MONI_SONAME) && $(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SONAME)

$(LIBSONAMEFULL):	$(TARGET)

static:	$(TARGET)
	echo Linking $@ ...
	$(COLLECT) $(LIBNAME) $(OBJECTS)
	$(RANLIB) $(LIBNAME)

$(TARGET)_gamma:	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(TARGET)_gamma.o
	echo Linking $@ ...
	$(CC) $(OPTS) -o $(TARGET)-gamma \
	$(TARGET)_gamma.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) -Wl,--rpath -Wl,$(libdir) \
	$(LDLIBS) 
	$(APPLE)

test2:	$(LIB_MONI_SONAMEFULL) test2.o
	echo Linking $@ ...
	$(CXX) $(OPTS) -o test2 \
	test2.o \
	$(LIB_MONI_SONAME) -Wl,--rpath -Wl,$(srcdir) \
	$(X11_LIBS) -l$(TARGET)
	$(APPLE)
test:	$(LIBSONAMEFULL) test.o
	$(CC) $(OPTS) -o test \
	test.o \
	$(LIBSONAMEFULL) -Wl,--rpath -Wl,$(srcdir) \
	$(LDLIBS) 
	$(APPLE)


install:	$(TARGET)
	cp -v $(TARGET) $(bindir)
clean:
	rm -v \
	$(OBJECTS) $(MONI_OBJECTS) $(LIBSONAMEFULL) $(LIBSONAME) $(LIB_MONI_SONAME)\
	$(LIB_MONI_SONAMEFULL) $(TARGET)_gamma.o $(TARGET)-gamma test2.o test.o \
	test2 test 

depend:
	$(MAKEDEPEND) -f.mkdep -I. $(CFLAGS) $(SOURCES)


# The extension to use for executables...
EXEEXT		= 

# Build commands and filename extensions...
.SUFFIXES:	.0 .1 .3 .c .cxx .h .fl .man .o .z $(EXEEXT)

.o$(EXEEXT):
	echo Linking $@ ...
	$(Cc) -I.. $(CFLAGS) $< $(LINKFLTK) $(LDLIBS) -o $@
	$(POSTBUILD) $@ ../FL/mac.r

.c.o:
	echo Compiling $< ...
	$(CC) -I.. $(CFLAGS) -c $<

.cxx.o:
	echo Compiling $< ...
	$(CXX) -I.. $(CXXFLAGS) -c $<

.cpp.o:
	echo Compiling $< ...
	$(CXX) -I.. $(CXXFLAGS) -c $<

tgz:
	tar cf - -C $(topdir) \
	$(addprefix $(dir)/,$(DOKU)) \
	$(dir)/makefile \
	$(addprefix $(dir)/,$(SOURCES)) \
	$(addprefix $(dir)/,$(FLUID)) \
	| gzip > $(TARGET)_$(mtime).tgz
	mv -v $(TARGET)_*.tgz ../Archiv

# makedepend
include .mkdep
