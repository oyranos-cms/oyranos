CC=c++
MAKEDEPEND	= /usr/X11R6//bin/makedepend -Y
OPTS=-Wall -g -O2

prefix		= /opt/local
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
datadir		= ${prefix}/share
includedir	= ${prefix}/include
libdir		= ${exec_prefix}/lib
mandir		= ${prefix}/man
srcdir		= .

#APPLE = 1
FLU = 1
DL = --ldflags # --ldstaticflags

ifdef FLU
FLU_H = -DHAVE_FLU
endif

CXXFLAGS=$(OPTS) $(INCL) $(FLU_H)
INCL=-I$(includedir) -I/usr/X11R6/include -I./

X11_LIBS=-L/usr/X11R6/lib -lXinerama -lXft

FLTK_LIBS=`fltk-config --use-images $(DL)`

KDB_LIBS=-lkdb

ifdef FLU
FLU_LIBS=`flu-config $(DL)`
endif


LDLIBS = -L$(libdir) -L./ $(FLTK_LIBS) \
	$(X11_LIBS) -llcms $(KDB_LIBS) $(FLU_LIBS)

#	$(VRML_LIBS)

CPP_HEADERS = \
	oyranos.h
#	fl_oyranos.h \
CPPFILES = \
	oyranos.cpp
CXXFILES =
#	fl_oyranos.cxx
DOKU = \
        README \
        ChangeLog \
        COPYING \
        AUTHORS
FLUID = #\
	fl_oyranos.fl

SOURCES = $(CPPFILES) $(CXXFILES) $(CPP_HEADERS)
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o)
TARGET  = oyranos

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
	$(CC) $(OPTS) -o $(TARGET) \
	$(OBJECTS) \
	$(LDLIBS)
	$(APPLE)

static:		$(OBJECTS)
	echo Linking $@...
	$(CC) $(OPTS) -o $(TARGET) \
	$(OBJECTS) \
	$(LDLIBS) -static -ljpeg -lpng -lX11 -lpthread -lz -ldl \
	-lfreetype -lfontconfig -lXrender -lXext -lexpat
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

