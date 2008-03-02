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
else
 ifdef APPLE
  COPY = cp -v
 else
  COPY = cp
 endif
endif
INSTALL = install -v
RPMARCH = `rpmbuild --showrc | awk '/^build arch/ {print $$4}'`

exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
datadir		= ${prefix}/share
includedir	= ${prefix}/include
ifdef BUILD_64
  libdir		= ${prefix}/lib64
else
  libdir		= ${prefix}/lib
endif
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
    OPTS=-Wall -g -fPIC #-O2
    LINK_FLAGS = -shared -ldl -fPIC -L.
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

INCL= -I/usr/include -I$(includedir) -I/usr/X11R6/include -I$(srcdir) \
	-I$(PNG_H) $(ELEKTRA_H)
CXXFLAGS=$(OPTS) $(INCL) $(FLU_H)
CFLAGS = $(OPTS) $(INCL)

ifdef BUILD_64
  X11_LIBS=-L/usr/X11R6/lib64 -lX11
else
  X11_LIBS=-L/usr/X11R6/lib -lX11
endif

#ELEKTRA_LIBS=-lelektra -lelektra_default


LDLIBS = -L$(libdir) -L. \
	$(ELEKTRA_LIBS) -ldl -lc $(PNG_LIBS) #-llcms $(FLTK_LIBS) $(FLU_LIBS)


CPP_HEADERS = \
	$(TARGET).h \
	$(TARGET)_config.h \
	$(TARGET)_debug.h \
	$(TARGET)_definitions.h \
	$(TARGET)_helper.h \
	$(TARGET)_internal.h \
	$(TARGET)_monitor.h
#	fl_$(TARGET).h
CFILES = \
	$(TARGET).c \
	$(TARGET)_helper.c
CFILESC = \
	$(TARGET)_debug.c
CFILES_MONI = \
    $(TARGET)_monitor.c
CFILES_GAMMA = \
    $(TARGET)_gamma.c

CPPFILES_FLU = \
	$(TARGET)_config_flu.cpp

CPPFILES =
CXXFILES =
#	fl_oyranos.cxx
DOKU = \
        AUTHORS \
        ChangeLog \
        COPYING \
        README \
		Doxyfile \
		doxymentation

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

timedir = .
mtime   := $(shell find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g)

#.SILENT:

ifdef FLU
FLU_GUI = $(TARGET)-config-flu
endif

STD_PROFILES = base eci

ALL_FILES =	$(DOKU) \
	configure \
	makefile \
	$(TARGET).pc.in \
	$(TARGET)_monitor.pc.in \
	$(TARGET).spec.in \
	$(TARGET)-config.in \
	$(SOURCES) \
	$(FLUID)

# build all what is needed to run the libraries, helpers and the examples
all:	config mkdepend $(TARGET) $(TARGET)_moni $(TARGET)-gamma $(FLU_GUI) test2
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         echo -e "$${prof} profiles will be included" || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;

# build all objects and libraries, link the headers to $(TARGET)
$(TARGET):	$(TARGET)/$(TARGET).h $(OBJECTS) static
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME) -o $(LIBSONAMEFULL) \
	$(OBJECTS) \
	-L/lib $(LDLIBS)
	$(REZ)
	$(RM)  $(LIBSONAME)
	$(LNK) $(LIBSONAMEFULL) $(LIBSONAME)
	$(RM)  $(LIBSO)
	$(LNK) $(LIBSONAMEFULL) $(LIBSO)

# the monitor library
$(TARGET)_moni:	$(MONI_OBJECTS) static_moni
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME_M) $(LIBSONAMEFULL) $(X11_LIBS) \
	-o $(LIB_MONI_SONAMEFULL) \
	$(MONI_OBJECTS) $(LDLIBS) 
	$(REZ)
	$(RM)  $(LIB_MONI_SONAME)
	$(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SONAME)
	$(RM)  $(LIB_MONI_SO)
	$(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SO)

# general configuration tool example
$(TARGET)-config-flu:	$(TARGET)_moni $(FLU_OBJECTS)
	echo Linking $@ ...
	$(CXX) $(OPTS) -o $(TARGET)-config-flu \
	$(TARGET)_config_flu.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(FLU_LIBS) $(FLTK_LIBS) $(LDLIBS) $(PNG_LIBS) \
	$(REZ)

# dangerous because of elektra dont allows static linking
$(TARGET)-config-flu-static:	$(TARGET)_moni $(FLU_OBJECTS)
	echo Linking static $(TARGET)-config-flu ...
	$(CXX) -Wall -O3 -o $(TARGET)-config-flu $(TARGET)_config_flu.o \
	$(LIBNAME) $(LIB_MONI_NAME) $(LINK_LIB_PATH) \
	`flu-config --ldstaticflags` \
	`fltk-config --use-images --ldstaticflags` \
	-L/usr/X11R6/lib \
	`test -f /usr/lib/libelektra.a && echo /usr/lib/libelektra.a || echo -lelektra` -lsupc++ $(PNG_LIBS)
	#strip $(TARGET)-config-flu
	$(REZ)

static:	$(OBJECTS)
	echo Linking $@ ...
	$(COLLECT) $(LIBNAME) $(OBJECTS)
	$(RANLIB) $(LIBNAME)

static_moni:	$(MONI_OBJECTS)
	echo Linking $@ ...
	$(COLLECT) $(LIB_MONI_NAME) $(MONI_OBJECTS)
	$(RANLIB) $(LIB_MONI_NAME)

# the monitor profile tool
$(TARGET)-gamma:	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(TARGET)_gamma.o
	echo Linking $@ ...
	$(CC) $(OPTS) -o $(TARGET)-gamma \
	$(TARGET)_gamma.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(LDLIBS) 
	$(REZ)

# bring the headers to one place
$(TARGET)/$(TARGET).h:
	mkdir $(TARGET)
	$(LNK) ../$(TARGET).h $(TARGET)/$(TARGET).h
	$(LNK) ../$(TARGET)_config.h $(TARGET)/$(TARGET)_config.h
	$(LNK) ../$(TARGET)_definitions.h $(TARGET)/$(TARGET)_definitions.h
	$(LNK) ../$(TARGET)_monitor.h $(TARGET)/$(TARGET)_monitor.h

test2:	$(LIB_MONI_SONAMEFULL) test2.o
	echo Linking $@ ...
	$(CXX) $(OPTS) -o test2 \
	test2.o \
	$(LIB_MONI_SONAMEFULL) $(LINK_SRC_PATH) \
	$(X11_LIBS) $(LDLIBS) -l$(TARGET)
	$(REZ)
test:	$(LIBSONAMEFULL) test.o
	$(CC) $(OPTS) -o test \
	test.o \
	$(LIBSONAMEFULL) $(LINK_SRC_PATH) \
	$(LDLIBS) 
	$(REZ)

doc:
	echo Documentation ...
	test -n 'which doxygen' && doxygen Doxyfile
	echo ... Documentation done

# the copy part for this directory level
install-main:	$(TARGET) $(TARGET)_moni $(TARGET)-gamma doc
	echo Installing ...
	-make uninstall
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-config     $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-gamma      $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)$(libdir)
	mkdir -p $(DESTDIR)$(libdir)/pkgconfig
	$(INSTALL) -m 755 $(TARGET).pc         $(DESTDIR)$(libdir)/pkgconfig/
	$(INSTALL) -m 755 $(TARGET)_monitor.pc $(DESTDIR)$(libdir)/pkgconfig/
	$(INSTALL) -m 644 $(LIBNAME) $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(LIBSONAMEFULL) $(DESTDIR)$(libdir)
	$(LNK)  $(LIBSONAMEFULL) $(DESTDIR)$(libdir)/$(LIBSONAME)
	$(LNK)  $(LIBSONAMEFULL) $(DESTDIR)$(libdir)/$(LIBSO)
	$(INSTALL) -m 644 $(LIB_MONI_NAME) $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(LIB_MONI_SONAMEFULL) $(DESTDIR)$(libdir)
	$(LNK)  $(LIB_MONI_SONAMEFULL) $(DESTDIR)$(libdir)/$(LIB_MONI_SONAME)
	$(LNK)  $(LIB_MONI_SONAMEFULL) $(DESTDIR)$(libdir)/$(LIB_MONI_SO)
	test -d $(DESTDIR)$(includedir)/$(TARGET) || mkdir -p $(DESTDIR)$(includedir)/$(TARGET)
	$(INSTALL) -m 644 $(TARGET).h $(DESTDIR)$(includedir)/$(TARGET)
	$(INSTALL) -m 644 $(TARGET)_config.h $(DESTDIR)$(includedir)/$(TARGET)
	$(INSTALL) -m 644 $(TARGET)_definitions.h $(DESTDIR)$(includedir)/$(TARGET)
	$(INSTALL) -m 644 $(TARGET)_monitor.h $(DESTDIR)$(includedir)/$(TARGET)
	test "$(FLU_GUI)" && make install_gui || echo -e "GUI not installed"
	echo ... Installation finished

# install recursive
install:	install-main
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         (cd $${prof}; make DESTDIR=$(DESTDIR) install) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;

install_gui:	$(TARGET)-config-flu
	echo Installing $(TARGET)-config-flu ...
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-config-flu $(DESTDIR)$(bindir)

# build a source distribution package
dist: targz
	test -d ../Archiv && $(COPY) ../Archiv/$(TARGET)-$(mtime).tgz $(TARGET)-$(VERSION).tar.gz || $(COPY) $(TARGET)-$(mtime).tgz $(TARGET)-$(VERSION).tar.gz

# build a binary rpm package
rpm:	dist
	mkdir -p rpmdir/BUILD \
	rpmdir/SPECS \
	rpmdir/SOURCES \
	rpmdir/SRPMS \
	rpmdir/RPMS/$(RPMARCH)
	cp -f $(TARGET)-$(VERSION).tar.gz rpmdir/SOURCES
	rpmbuild --clean -ba $(srcdir)/$(TARGET).spec --define "_topdir $$PWD/rpmdir"
	@echo "============================================================"
	@echo "Finished - the packages are in rpmdir/RPMS and rpmdir/SRPMS!"
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         (cd $${prof}; echo bin in `pwd`; make rpm) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;

# remove everything previously installed
uninstall:
	echo Uninstalling ...
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-gamma
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-config
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-config-flu
	$(RM)   $(DESTDIR)$(libdir)/pkgconfig/$(TARGET).pc
	$(RM)   $(DESTDIR)$(libdir)/pkgconfig/$(TARGET)_monitor.pc
	$(RM)   $(DESTDIR)$(libdir)/$(LIBSONAMEFULL) \
	        $(DESTDIR)$(libdir)/$(LIBSONAME) \
	        $(DESTDIR)$(libdir)/$(LIBSO) \
			$(DESTDIR)$(libdir)/$(LIBNAME)
	$(RM)   $(DESTDIR)$(libdir)/$(LIB_MONI_SONAMEFULL) \
	        $(DESTDIR)$(libdir)/$(LIB_MONI_SONAME) \
			$(DESTDIR)$(libdir)/$(LIB_MONI_SO) \
	        $(DESTDIR)$(libdir)/$(LIB_MONI_NAME)
	$(RM)   -R $(DESTDIR)$(includedir)/$(TARGET)
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         (cd $${prof}; make uninstall) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;

# remove in this directory
clean:
	$(RM) \
	$(OBJECTS) $(MONI_OBJECTS) \
	$(LIBNAME) $(LIBSONAMEFULL) $(LIBSONAME) $(LIB_SO) \
	$(LIB_MONI_NAME) $(LIB_MONI_SONAME) $(LIB_MONI_SO) $(LIB_MONI_SONAMEFULL) \
	$(TARGET)_gamma.o $(TARGET)-gamma test2.o test.o test2 test \
	$(TARGET)-config-flu $(FLU_OBJECTS) $(TARGET)_version.h config.h \
	$(TARGET)-config config mkdepend

# configure if the file config is not available
config:
	./configure

# try to resolve dependencies with the X11 tool
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

# smallest package covering the current directory
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

# build the source package including the subdirectories
targz:
	test -d $(TARGET)-$(VERSION) && $(RM) -R $(TARGET)-$(VERSION) || echo -e "\c"
	mkdir $(TARGET)-$(VERSION)
	$(COPY) \
	$(ALL_FILES) \
	$(TARGET)-$(VERSION)
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         mkdir -p ../$(TARGET)-$(VERSION)/standard_profiles/$${prof}; \
	         (cd $${prof}; echo bin in `pwd`; make DESTDIR=../../$(TARGET)-$(VERSION)/standard_profiles/$${prof} copy) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;
	tar cf - $(TARGET)-$(VERSION)/ \
	| gzip > $(TARGET)-$(mtime).tgz
	test -d $(TARGET)-$(VERSION) && \
	test `pwd` != `(cd $(TARGET)-$(VERSION); pwd)` && \
	$(RM) -R $(TARGET)-$(VERSION)
	test -d ../Archiv && mv -v $(TARGET)-*.tgz ../Archiv || echo "no copy"


# mkdepend
include mkdepend

ifndef MAKEDEPEND_ISUP
mkdepend: depend
endif

