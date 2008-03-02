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

ifdef BUILD_64
  LIB=/lib64
else
  LIB=/lib
endif

exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
datadir		= ${prefix}/share
includedir	= ${prefix}/include
libdir		= ${prefix}$(LIB)
mandir		= ${prefix}/man
srcdir		= .
colordir        = /usr/share/color

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
LIB_XNVCTRL = libXNVCtrl
LIB_XNVCTRL_NAME = $(LIB_XNVCTRL).a

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
    LINK_NAME = -Wl,-soname -Wl,$(LIBSONAMEFULL)
    LINK_NAME_M = -Wl,-soname -Wl,$(LIB_MONI_SONAMEFULL)
    LINK_LIB_PATH = -Wl,--rpath -Wl,$(libdir)
    LINK_SRC_PATH = -Wl,--rpath -Wl,$(srcdir)
  else
    OPTS=-Wall -O2 -g -fpic -L.
    LINK_FLAGS = -shared -ldl $(ICONV)
    RM = rm -f
  endif
endif

INCL= -I/usr/include -I$(includedir) -I/usr/X11R6/include -I$(srcdir) \
	$(PNG_H) $(ELEKTRA_H)
CXXFLAGS=$(OPTS) $(INCL) $(FLU_H)
CFLAGS = $(OPTS) $(INCL)

X11_LIBS=-L/usr/X11R6$(LIB) -lX11 -lXext -lXinerama

#ELEKTRA_LIBS=-lelektra -lelektra_default


LDLIBS = -L$(libdir) -L. \
	$(ELEKTRA_LIBS) -ldl -lc


CPP_HEADERS = \
	$(TARGET).h \
	$(TARGET)_config.h \
	$(TARGET)_debug.h \
	$(TARGET)_definitions.h \
	$(TARGET)_helper.h \
	$(TARGET)_internal.h \
	$(TARGET)_monitor.h \
	$(TARGET)_monitor_internal.h
#	fl_$(TARGET).h
CFILES = \
	$(TARGET).c \
	$(TARGET)_helper.c
CFILESC = \
	$(TARGET)_debug.c
CFILES_MONI = \
	$(TARGET)_monitor.c
CFILES_MONI_NVIDIA = \
	$(TARGET)_monitor_nvidia.c
CFILES_GAMMA = \
	$(TARGET)_gamma.c

CXXFILES_FLTK = \
	$(TARGET)_config_fltk.cxx
CPPFILES_FLU = \
	$(TARGET)_config_flu.cpp

CPPFILES =
CXXFILES =
#	fl_oyranos.cxx
DATA = \
	$(TARGET)_logo.png
DOKU = \
        AUTHORS \
        ChangeLog \
        COPYING \
        README \
		Doxyfile \
		doxymentation

FLUID = \
	oyranos_config_fltk.fl

SOURCES = $(CPPFILES) $(CXXFILES) $(CPP_HEADERS) $(CFILES) $(CFILESC) \
		$(CFILES_MONI) $(CFILES_MONI_NVIDIA) $(CFILES_GAMMA) \
		$(CPPFILES_FLU) test.c test2.cpp
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o) $(CFILES:.c=.o) $(CFILESC:.c=.o)
MONI_OBJECTS = $(CPPFILES_MONI:.cpp=.o) $(CXXFILESMONI:.cxx=.o) $(CFILES_MONI:.c=.o)
FLTK_OBJECTS = $(CXXFILES_FLTK:.cxx=.o)
FLU_OBJECTS = $(CPPFILES_FLU:.cpp=.o) $(CXXFILES_FLU:.cxx=.o) \
				$(CFILES_FLU:.c=.o)

INCL_DEP = $(INCL) $(SOURCES)

ifdef APPLE
REZ     = /Developer/Tools/Rez -t APPL -o $(TARGET) mac.r
endif

timedir = .
mtime   := $(shell find $(timedir) -prune -printf %Ty%Tm%Td.%TT | sed s/://g)

.SILENT:

ifdef FLTK
FLTK_GUI = $(TARGET)-config-fltk
endif
ifdef FLU
FLU_GUI = $(TARGET)-config-flu
endif

STD_PROFILES = base eci

ALL_FILES = \
	$(DATA) \
	$(DOKU) \
	configure \
	makefile \
	$(TARGET).pc.in \
	$(TARGET)_monitor.pc.in \
	$(TARGET).spec.in \
	$(TARGET)-config.in \
	$(SOURCES) \
	$(FLUID)

# build all what is needed to run the libraries, helpers and the examples
all:	config mkdepend $(TARGET) $(TARGET)_moni $(TARGET)-monitor $(FLU_GUI) \
	$(FLTK_GUI) test2
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         echo -e "$${prof} profiles will be included" || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;

# build all objects and libraries, link the headers to $(TARGET)
$(TARGET):	$(OBJECTS) static
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME) -o $(LIBSONAMEFULL) \
	$(OBJECTS) \
	-L$(LIB) $(LDLIBS)
	$(REZ)
	$(RM)  $(LIBSONAME)
	$(LNK) $(LIBSONAMEFULL) $(LIBSONAME)
	$(RM)  $(LIBSO)
	$(LNK) $(LIBSONAMEFULL) $(LIBSO)

# the monitor library
$(TARGET)_moni:	$(MONI_OBJECTS) static_moni
	echo Linking $@ ...
	$(CC) $(OPTS) $(LINK_FLAGS) $(LINK_NAME_M) $(LIBSONAMEFULL) \
	-o $(LIB_MONI_SONAMEFULL) \
	$(MONI_OBJECTS) $(LDLIBS) $(X11_LIBS)
	$(REZ)
	$(RM)  $(LIB_MONI_SONAME)
	$(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SONAME)
	$(RM)  $(LIB_MONI_SO)
	$(LNK) $(LIB_MONI_SONAMEFULL) $(LIB_MONI_SO)

$(LIB_XNVCTRL).a:
	-(cd $(LIB_XNVCTRL) && make)

# the twinview library
$(TARGET)-monitor-nvidia:
	$(CC) $(OPTS) $(TARGET)_monitor_nvidia.c -I./$(LIB_XNVCTRL) \
	-o $(TARGET)-monitor-nvidia -L./   $(LDLIBS) $(X11_LIBS) \
	-L./$(LIB_XNVCTRL) -lXNVCtrl $(LIB_MONI_SONAMEFULL)

$(LIB_XNVCTRL):	$(LIB_XNVCTRL).a
	-make $(TARGET)-monitor-nvidia

# general configuration tool example
$(TARGET)-config-fltk:	$(TARGET)_moni $(FLTK_OBJECTS)
	echo Linking $@ ...
	$(CXX) $(OPTS) -o $(TARGET)-config-fltk \
	$(FLTK_OBJECTS) \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(FLTK_LIBS) $(LDLIBS) $(PNG_LIBS) \
	$(REZ)

$(TARGET)_config_fltk.o:	$(TARGET)_config_fltk.cxx
	$(CXX) -I.. $(CXXFLAGS) $(FLTK_H) -c $<

$(TARGET)-config-flu:	$(TARGET)_moni $(FLU_OBJECTS)
	echo Linking $@ ...
	$(CXX) $(OPTS) -o $(TARGET)-config-flu \
	$(TARGET)_config_flu.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(FLU_LIBS) $(FLTK_LIBS) $(LDLIBS) $(PNG_LIBS) \
	$(REZ)


$(TARGET)-config-flu-static:	$(TARGET)_moni $(FLU_OBJECTS)
	echo Linking static $(TARGET)-config-flu ...
	$(CXX) -Wall -O3 -o $(TARGET)-config-flu $(TARGET)_config_flu.o \
	$(LIBNAME) $(LIB_MONI_NAME) $(LINK_LIB_PATH) \
	`flu-config --ldstaticflags` \
	`fltk-config --use-images --ldstaticflags` \
	-L/usr/X11R6$(LIB) \
	`test -f /usr$(LIB)/libelektra.a && echo /usr$(LIB)/libelektra.a || echo -lelektra` -lsupc++ $(PNG_LIBS)
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
$(TARGET)-monitor:	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(TARGET)_gamma.o $(LIB_XNVCTRL)
	echo Linking $@ ...
	$(CC) $(OPTS) -o $(TARGET)-monitor \
	$(TARGET)_gamma.o \
	$(LIBSONAMEFULL) $(LIB_MONI_SONAMEFULL) $(LINK_LIB_PATH) \
	$(LDLIBS)
	$(REZ)

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
install-main:	$(TARGET) $(TARGET)_moni $(TARGET)-monitor doc
	echo Installing ...
	-make uninstall
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-config     $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-monitor    $(DESTDIR)$(bindir)
	-$(INSTALL) -m 755 $(TARGET)-monitor-nvidia $(DESTDIR)$(bindir)
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
	test "$(FLTK_GUI)" && make install_gui || echo -e "GUI not installed"
	echo Installing policy settings files ...
	-mkdir -p $(DESTDIR)$(colordir)/settings
	-$(INSTALL) -m 644 settings/*.policy.xml $(DESTDIR)$(colordir)/settings
	echo ... Installation finished

# install recursive
install:	install-main
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         (cd $${prof}; make DESTDIR=$(DESTDIR) install) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;

install_gui:	$(TARGET)-config-flu $(TARGET)-config-fltk
	echo Installing UI ...
	mkdir -p $(DESTDIR)$(bindir)
	-$(INSTALL) -m 755 $(TARGET)-config-flu $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(TARGET)-config-fltk $(DESTDIR)$(bindir)

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
	cd standard_profiles; \
	for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         (cd $${prof}; echo bin in `pwd`; make rpm) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	done;
	cp -f $(TARGET)-$(VERSION).tar.gz rpmdir/SOURCES
	rpmbuild --clean -ba $(srcdir)/$(TARGET).spec --define "_topdir $$PWD/rpmdir"
	@echo "============================================================"
	@echo "Finished - the packages are in rpmdir/RPMS and rpmdir/SRPMS!"

# remove everything previously installed
uninstall:
	echo Uninstalling ...
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-monitor
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-config
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-config-flu
	$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-config-fltk
	-$(RM)   $(DESTDIR)$(bindir)/$(TARGET)-monitor-nvidia
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
	test -d standard_profiles && \
	(  cd standard_profiles; \
	   for prof in $(STD_PROFILES); do \
	     test $${prof} && \
	         (cd $${prof}; make uninstall) || \
	         echo -e "$${prof} profile directory is not found - ignoring"; \
	   done; \
        ) || echo "no profile directory found"
	-$(RM)   $(DESTDIR)$(colordir)/settings/*.policy.xml

# remove in this directory
clean:
	$(RM) \
	$(OBJECTS) $(MONI_OBJECTS) \
	$(LIBNAME) $(LIBSONAMEFULL) $(LIBSONAME) $(LIB_SO) \
	$(LIB_MONI_NAME) $(LIB_MONI_SONAME) $(LIB_MONI_SO) $(LIB_MONI_SONAMEFULL) \
	$(TARGET)_gamma.o $(TARGET)-monitor $(TARGET)-monitor-nvidia \
	test2.o test.o test2 test \
	$(TARGET)-config-fltk $(FLTK_OBJECTS) \
	$(TARGET)-config-flu $(FLU_OBJECTS) $(TARGET)_version.h config.h \
	$(TARGET)-config config mkdepend
	-(cd $(LIB_XNVCTRL) && make clean)

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

.fl.cxx:
	echo Expanding $< ...
	fluid -c $<

# smallest package covering the current directory
tgz:
	make DESTDIR=Entwickeln copy_files
	tar cf - Entwickeln/ \
	| gzip > $(TARGET)_$(mtime).tgz
	test -d ../Archiv && mv -v $(TARGET)_*.tgz ../Archiv
	test -d Entwickeln && \
	test `pwd` != `(cd Entwickeln; pwd)` && \
	rm -R Entwickeln

# build the source package including the subdirectories
targz:
	test -d $(TARGET)-$(VERSION) && $(RM) -R $(TARGET)-$(VERSION) || echo -e "\c"
	make DESTDIR=$(TARGET)-$(VERSION) copy_files
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

# basic file set
copy_files:
	mkdir $(DESTDIR)
	$(COPY) -R \
	$(ALL_FILES) \
	$(DESTDIR)
	$(COPY) -r settings $(DESTDIR)
	-(cd $(LIB_XNVCTRL) && make clean)
	$(COPY) -R $(LIB_XNVCTRL) $(DESTDIR)

# mkdepend
include mkdepend

ifndef MAKEDEPEND_ISUP
mkdepend: depend
endif

