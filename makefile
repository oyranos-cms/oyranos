CC=cc
CXX=c++
MAKEDEPEND	= /usr/X11R6//bin/makedepend -Y
OPTS=-Wall -g -O2
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

#APPLE = 1
FLU = 1
DL = --ldflags # --ldstaticflags

ifdef FLU
FLU_H = -DHAVE_FLU
endif

LINK_FLAGS = -shared -Wl,-soname -Wl,$(LIBSONAME)

CXXFLAGS=$(OPTS) $(INCL) $(FLU_H)
INCL=-I$(includedir) -I/usr/X11R6/include -I$(srcdir)
CFLAGS = $(OPTS) $(INCL)

X11_LIBS=#-L/usr/X11R6/lib -lXinerama -lXft

FLTK_LIBS=#`fltk-config --use-images $(DL)`

KDB_LIBS=-lkdb

ifdef FLU
FLU_LIBS=#`flu-config $(DL)`
endif


LDLIBS = -L$(libdir) -L./ $(FLTK_LIBS) \
	$(X11_LIBS) $(KDB_LIBS) #-llcms $(FLU_LIBS)


CPP_HEADERS = \
	oyranos.h \
	oyranos_definitions.h \
	oyranos_helper.h
#	fl_oyranos.h
CFILES = \
	oyranos.c
CPPFILES =
CXXFILES =
#	fl_oyranos.cxx
DOKU = \
        README \
        ChangeLog \
        COPYING \
        AUTHORS
FLUID = #\
	fl_oyranos.fl

SOURCES = $(CPPFILES) $(CXXFILES) $(CPP_HEADERS) $(CFILES) test.c
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o) $(CFILES:.c=.o)

REZ     = /Developer/Tools/Rez -t APPL -o $(TARGET) /opt/local/include/FL/mac.r
ifdef APPLE
APPLE   = $(REZ)
endif

topdir  = ..
dir     = Entwickeln
timedir = $(topdir)/$(dir)
mtime   = `find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g`

#.SILENT:

all:	$(TARGET)

$(TARGET):	$(OBJECTS)
	echo Linking $@...
	$(CC) $(OPTS) $(LINK_FLAGS) -o $(LIBSONAMEFULL) \
	$(OBJECTS) \
	$(LDLIBS) \
	$(APPLE)
	$(RM) $(LIBSONAME) && $(LNK) $(LIBSONAMEFULL) $(LIBSONAME)

$(LIBSONAMEFULL):	$(TARGET)

static:	$(TARGET)
	echo Linking $@...
	$(COLLECT) $(LIBNAME) $(OBJECTS)
	$(RANLIB) $(LIBNAME)

test:	$(LIBSONAMEFULL) test.o
	$(CC) $(OPTS) -o test \
	test.o \
	$(LIBSONAMEFULL) -Wl,--rpath -Wl,$(srcdir) \
	$(LDLIBS) 
	$(APPLE)


install:	$(TARGET)
	cp -v $(TARGET) $(bindir)
clean:
	rm -v $(OBJECTS) $(TARGET)

depend:
	$(MAKEDEPEND) -fmakedepend -I.. $(SOURCES) $(CFILES) $(CPP_HEADERS)


# The extension to use for executables...
EXEEXT		= 

# Build commands and filename extensions...
.SUFFIXES:	.0 .1 .3 .c .cxx .h .fl .man .o .z $(EXEEXT)

.o$(EXEEXT):
	echo Linking $@...
	$(CXX) -I.. $(CXXFLAGS) $< $(LINKFLTK) $(LDLIBS) -o $@
	$(POSTBUILD) $@ ../FL/mac.r

.c.o:
	echo Compiling $<...
	$(CC) -I.. $(CFLAGS) -c $<

.cxx.o:
	echo Compiling $<...
	$(CXX) -I.. $(CXXFLAGS) -c $<

.cpp.o:
	echo Compiling $<...
	$(CXX) -I.. $(CXXFLAGS) -c $<

tgz:
	tar cf - -C $(topdir) \
	$(addprefix $(dir)/,$(SOURCES)) \
	$(dir)/makefile \
	$(addprefix $(dir)/,$(DOKU)) \
	$(addprefix $(dir)/,$(FLUID)) \
	| gzip > $(TARGET)_$(mtime).tgz
	mv -v $(TARGET)_*.tgz ../Archiv

