include config

CC=cc
CXX=c++
COLLECT = ar cru
RANLIB = ranlib
MAKEDEPEND	= makedepend -Y
LNK = ln -s
RM = rm -vf
ifdef LINUX
COPY = cp -vdpa
endif
ifdef APPLE
COPY = cp -v
else
COPY = cp
endif
INSTALL = install -v
RPMARCH = `rpmbuild --showrc | awk '/^build arch/ {print $$4}'`

exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
datadir		= ${prefix}/share
includedir	= ${prefix}/include
libdir		= ${prefix}/lib
mandir		= ${prefix}/man
srcdir		= .

ifdef LINUX
SO = .so
endif

LIBSONAMEFULL = lib$(TARGET)$(SO).$(VERSION)
LIBSONAME = lib$(TARGET)$(SO).$(VERSION_A)
LIBSO = lib$(TARGET)$(SO)
LIBNAME = lib$(TARGET).a
LIB_MONI_SONAMEFULL = lib$(TARGET)_moni$(SO).$(VERSION)
LIB_MONI_SONAME = lib$(TARGET)_moni$(SO).$(VERSION_A)
LIB_MONI_SO = lib$(TARGET)_moni$(SO)
LIB_MONI_NAME = lib$(TARGET)_moni.a

DL = --ldflags # --ldstaticflags
ICONV = -liconv



ifdef APPLE
  OPTS=-Wall -O2 -g -fPIC -L.
  LINK_FLAGS = -dynamiclib
  MAKEDEPEND	= /usr/X11R6/bin/makedepend -Y
else
  SO = .so
  ifdef LINUX
    OPTS=-Wall -O2 -g
    LINK_FLAGS = -shared -ldl -L.
    LINK_FLAGS_STATIC = q
    LINK_NAME = -Wl,-soname -Wl,$(LIBSONAME)
    LINK_NAME_M = -Wl,-soname -Wl,$(LIB_MONI_SONAME)
    LINK_LIB_PATH = -Wl,--rpath -Wl,$(libdir)
    LINK_SRC_PATH = -Wl,--rpath -Wl,$(srcdir)
  else
    OPTS=-Wall -O2 -g -fpic -L.
    LINK_FLAGS = -shared -ldl $(ICONV)
    RM = rm -f
  endif
endif

INCL= -I/usr/include -I$(includedir) -I/usr/X11R6/include -I$(srcdir)
CXXFLAGS=$(OPTS) $(INCL) $(FLU_H)
CFLAGS = $(OPTS) $(INCL)

X11_LIBS=-L/usr/X11R6/lib -lX11

KDB_LIBS=-lkdb


LDLIBS = -L$(libdir) -L. \
	$(KDB_LIBS) -ldl -lc #-llcms $(FLTK_LIBS) $(FLU_LIBS)


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

CPPFILES_FLU = \
	oyranos_config_flu.cpp

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
		  $(CFILES_MONI) $(CFILES_GAMMA) $(CPPFILES_FLU) test.c test2.cpp
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o) $(CFILES:.c=.o) $(CFILESC:.c=.o)
MONI_OBJECTS = $(CPPFILES_MONI:.cpp=.o) $(CXXFILESMONI:.cxx=.o) $(CFILES_MONI:.c=.o)
FLU_OBJECTS = $(CPPFILES_FLU:.cpp=.o) $(CXXFILES_FLU:.cxx=.o) \
				$(CFILES_FLU:.c=.o)

INCL_DEP = $(INCL) $(SOURCES)

ifdef APPLE
REZ     = /Developer/Tools/Rez -t APPL -o $(TARGET) mac.r
endif

topdir  = ..
dir     = Entwickeln
timedir = $(topdir)/$(dir)
mtime   = `find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g`

.SILENT:

ifdef FLU
FLU_GUI = $(TARGET)_config_flu
endif

ALL_FILES =	$(DOKU) \
	configure \
	makefile \
	oyranos.spec \
	oyranos-config.in \
	$(SOURCES) \
	$(FLUID)

all:	config mkdepend $(TARGET) $(TARGET)_moni $(TARGET)_gamma $(FLU_GUI) test2

$(TARGET):	$(OBJECTS) static
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME) -o $(LIBSONAMEFULL) \
	$(OBJECTS) \
	$(LDLIBS)
	$(REZ)
	$(RM)  $(LIBSONAME)
	$(LNK) $(LIBSONAMEFULL) $(LIBSONAME)
	$(RM)  $(LIBSO)
	$(LNK) $(LIBSONAMEFULL) $(LIBSO)

$(TARGET)_moni:	$(MONI_OBJECTS) static_moni
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME_M) $(X11_LIBS) \
	-o $(LIB_MONI_SONAMEFULL) \
	$(MONI_OBJECTS) $(LDLIBS) 
	$(REZ)
	$(RM)  $(LIB_MONI_SONAME)
	$(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SONAME)
	$(RM)  $(LIB_MONI_SO)
	$(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SO)

$(TARGET)_config_flu:	$(TARGET)_moni $(FLU_OBJECTS)
	echo Linking $@ ...
	$(CXX) $(OPTS) -o $(TARGET)_config_flu \
	$(TARGET)_config_flu.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(FLU_LIBS) $(FLTK_LIBS) $(LDLIBS) \
	$(REZ)

$(TARGET)_config_flu_static:	$(TARGET)_moni $(FLU_OBJECTS)
	echo Linking static $(TARGET)_config_flu ...
	$(CXX) -Wall -O3 -o $(TARGET)_config_flu $(TARGET)_config_flu.o \
	$(LIBNAME) $(LIB_MONI_NAME) $(LINK_LIB_PATH) \
	`flu-config --ldstaticflags` \
	`fltk-config --use-images --ldstaticflags` \
	-L/usr/X11R6/lib \
	/usr/lib/libkdb.a -lsupc++
	strip $(TARGET)_config_flu
	$(REZ)

static:	$(OBJECTS)
	echo Linking $@ ...
	$(COLLECT) $(LIBNAME) $(OBJECTS)
	$(RANLIB) $(LIBNAME)

static_moni:	$(MONI_OBJECTS)
	echo Linking $@ ...
	$(COLLECT) $(LIB_MONI_NAME) $(MONI_OBJECTS)
	$(RANLIB) $(LIB_MONI_NAME)

$(TARGET)_gamma:	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(TARGET)_gamma.o
	echo Linking $@ ...
	$(CC) $(OPTS) -o $(TARGET)-gamma \
	$(TARGET)_gamma.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(LDLIBS) 
	$(REZ)

test2:	$(LIB_MONI_SONAMEFULL) test2.o
	echo Linking $@ ...
	$(CXX) $(OPTS) -o test2 \
	test2.o \
	$(LIB_MONI_SONAME) $(LINK_SRC_PATH) \
	$(X11_LIBS) $(LDLIBS) -l$(TARGET)
	$(REZ)
test:	$(LIBSONAMEFULL) test.o
	$(CC) $(OPTS) -o test \
	test.o \
	$(LIBSONAMEFULL) $(LINK_SRC_PATH) \
	$(LDLIBS) 
	$(REZ)


install:	$(TARGET) $(TARGET)_moni $(TARGET)_gamma
	echo Installing ...
	make uninstall
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-config $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-gamma $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(LIBNAME) $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(LIBSONAMEFULL) $(DESTDIR)$(libdir)
	$(LNK)  $(LIBSONAMEFULL) $(DESTDIR)$(libdir)/$(LIBSONAME)
	$(LNK)  $(LIBSONAMEFULL) $(DESTDIR)$(libdir)/$(LIBSO)
	$(INSTALL) -m 644 $(LIB_MONI_NAME) $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(LIB_MONI_SONAMEFULL) $(DESTDIR)$(libdir)
	$(LNK)  $(LIB_MONI_SONAMEFULL) $(DESTDIR)$(libdir)/$(LIB_MONI_SONAME)
	$(LNK)  $(LIB_MONI_SONAMEFULL) $(DESTDIR)$(libdir)/$(LIB_MONI_SO)
	test -d $(includedir)/oyranos || mkdir -p $(DESTDIR)$(includedir)/oyranos
	$(INSTALL) -m 644 oyranos.h $(DESTDIR)$(includedir)/oyranos
	$(INSTALL) -m 644 oyranos_definitions.h $(DESTDIR)$(includedir)/oyranos
	$(INSTALL) -m 644 oyranos_monitor.h $(DESTDIR)$(includedir)/oyranos
	echo ... Installation finished

install_gui:	$(TARGET)_config_flu_static
	echo Installing $(TARGET)_config_flu_static ...
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)_config_flu $(DESTDIR)$(bindir)

dist: targz
	$(COPY) ../Archiv/$(TARGET)_$(mtime).tgz $(TARGET)_$(VERSION).tar.gz

rpm:	dist
	mkdir -p rpmdir/BUILD \
	rpmdir/SPECS \
	rpmdir/SOURCES \
	rpmdir/SRPMS \
	rpmdir/RPMS/$(RPMARCH)
	cp -f $(TARGET)_$(VERSION).tar.gz rpmdir/SOURCES
	rpmbuild --clean -ba $(srcdir)/$(TARGET).spec --define "_topdir $$PWD/rpmdir"
	@echo "============================================================"
	@echo "Finished - the packages are in rpmdir/RPMS and rpmdir/SRPMS!"

uninstall:
	echo Uninstalling ...
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-gamma
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-config
	$(RM)   $(DESTDIR)$(libdir)/$(LIBSONAMEFULL) \
	        $(DESTDIR)$(libdir)/$(LIBSONAME) \
	        $(DESTDIR)$(libdir)/$(LIBSO) \
			$(DESTDIR)$(libdir)/$(LIBNAME)
	$(RM)   $(DESTDIR)$(libdir)/$(LIB_MONI_SONAMEFULL) \
	        $(DESTDIR)$(libdir)/$(LIB_MONI_SONAME) \
			$(DESTDIR)$(libdir)/$(LIB_MONI_SO) \
	        $(DESTDIR)$(libdir)/$(LIB_MONI_NAME)
	$(RM)   -R $(DESTDIR)$(includedir)/oyranos

clean:
	$(RM) \
	$(OBJECTS) $(MONI_OBJECTS) \
	$(LIBNAME) $(LIBSONAMEFULL) $(LIBSONAME) $(LIB_SO) \
	$(LIB_MONI_NAME) $(LIB_MONI_SONAME) $(LIB_MONI_SO) $(LIB_MONI_SONAMEFULL) \
	$(TARGET)_gamma.o $(TARGET)-gamma test2.o test.o test2 test \
	$(TARGET)_config_flu $(FLU_OBJECTS) $(TARGET)_version.h config.h \
	$(TARGET)-config config mkdepend

config:
	configure

depend:
	echo "setting up dependencies ..."
	echo "MAKEDEPEND_ISUP = 1" > mkdepend
	$(MAKEDEPEND) -f mkdepend \
	-s "#dont edit - automatically generated" \
	-I. $(INCL_DEP)


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

.cpp.o:	mkdepend
	echo Compiling $< ...
	$(CXX) -I.. $(CXXFLAGS) -c $<

tgz:
	mkdir Entwickeln
	$(COPY) \
	$(ALL_FILES) \
	Entwickeln
	tar cf - Entwickeln/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d Entwickeln && \
	test `pwd` != `(cd Entwickeln; pwd)` && \
	rm -R Entwickeln

targz:
	mkdir $(TARGET)_$(VERSION)
	$(COPY) \
	$(ALL_FILES) \
	$(TARGET)_$(VERSION)
	tar cf - $(TARGET)_$(VERSION)/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d $(TARGET)_$(VERSION) && \
	test `pwd` != `(cd $(TARGET)_$(VERSION); pwd)` && \
	rm -R $(TARGET)_$(VERSION) 


# mkdepend
include mkdepend

ifndef MAKEDEPEND_ISUP
mkdepend: depend
endif

