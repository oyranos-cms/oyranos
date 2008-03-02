
LINUX = 1
CXX = c++
CC = cc
dl = -ldl
X11 = X11
X_H = -I/usr/X11R6/include -I/usr/include
XF86VMODE = 1
XF86VMODE_LIB = -lXxf86vm
XIN = 1
XINERAMA_LIB = -lXinerama
X11_LIB_PATH = -L/usr/X11R6/lib$(BARCH) -L/usr/lib$(BARCH) -L$(libdir)
X11_LIBS=$(X11_LIB_PATH) -lX11 $(XF86VMODE_LIB) -lX11 -lXext -lXinerama -lXxf86vm $(XINERAMA_LIB)
HAVE_FLTK_UTF8 = -DHAVE_FLTK_UTF8
FLTK = 1
FLTK_H = -I/data3/CD/programme/fltk118-utf8-2007-07-14 -I/usr/include/freetype2 -Wall -g -I/usr/X11R6/include -D_THREAD_SAFE -D_REENTRANT
FLTK_LIBS = -L/data3/CD/programme/fltk118-utf8-2007-07-14/lib -L/usr/X11R6/lib -Wall -g -L/usr/X11R6/lib -lfltk-utf8_images -lpng -lz -ljpeg -lfltk-utf8_gl -lGLU -lGL -lfltk-utf8 -lfltk_xutf8 -lXft -lXinerama -lpthread -ldl -lm -lXext -lX11 
fltkconfig = /data3/CD/programme/fltk118-utf8-2007-07-14/fltk-utf8-config
GETTEXT = -DUSE_GETTEXT
DEBUG = -Wall -g -DDEBUG --pedantic
DEBUG_SWITCH = -v
CFLAGS =  $(DEBUG) $(INCL) -Wall -g -I/opt/local/include -fPIC -DPIC
CXXFLAGS =  $(DEBUG) $(INCL) $(FLTK_H) -Wall -g -I/opt/local/include -fPIC -DPIC
LDFLAGS = -Wall -g -L/usr/X11R6/lib -L/opt/local/lib -L/opt/local/lib -L. $(dl)
TARGET=fl_i18n
VERSION=0.0.2
COMPILE_OPTIONS=-DUSE_THREADS $(GETTEXT)

BINTARGET = $(TARGET)_example


COLLECT = ar cru
RANLIB = ranlib
MAKEDEPEND	= $(CXX) -MM
LINK = ln -s
RM = rm -vf
INSTALL = install -v


I18N_CXXFILES = $(TARGET).cxx
I18N_HEADERS = $(TARGET).H
MSGFMT = msgfmt -c --statistics


LIB_LIBS = -L./ $(FLTK_LIBS) \
	-L/lib \
	$(DBG_LIBS)

ifdef APPLE
  LIBEXT = .dylib
  OPTS=-Wall -g $(DEBUG) -DPIC -Wunused -fno-exceptions $(COMPILE_OPTIONS)
  LIBLINK_FLAGS = -dynamiclib $(LIB_LIBS) 
  I18N_LIB = $(intl) $(LIBNAME) #-liconv
  I18N_LIBSTAT = /opt/local/lib/libintl.a $(LIBNAME) #-liconv
  INCL=-I$(includedir) -I/usr/X11R6/include -I./ -I/usr/include/gcc/darwin/default/c++
  REZ = $(fltkconfig) --post $@;
  DBG_LIBS = #-lMallocDebug
  MSGMERGE := msgmerge
  XGETTEXT_OPTIONS = --keyword=gettext \
	--keyword=_ \
	--keyword=N_
  COPY = cp -v
else
  SO = .so
    MSGMERGE = msgmerge
    MSGMERG_OPTS = --update
    XGETTEXT_OPTIONS = --keyword=gettext --flag=gettext:1:pass-c-format \
	--keyword=_ --flag=_:1:pass-c-format \
	--keyword=N_ --flag=N_:1:pass-c-format \
	--copyright-holder='Kai-Uwe Behrmann' \
	--msgid-bugs-address='ku.b@gmx.de' \
	--from-code=utf-8
ifdef LINUX
    OPTS = -Wall -g $(DEBUG) -Wunused -fPIC -fno-exceptions $(COMPILE_OPTIONS)
    INCL=-I$(includedir) -I/usr/X11R6/include -I. -I/usr/include/g++ -I/usr/include
    I18N_LIB = $(LIBNAME)
    COPY = cp -vdpa
else
    OPTS=-Wall -g $(DEBUG) -Wunused -fno-exceptions -lc -lm $(COMPILE_OPTIONS)
    INCL= -I./
    RM = rm -f
    COPY = cp
    LIBLINK_FLAGS = -shared $(dl) -L./
    I18N_LIB = $(iconv) $(intl) $(LIBNAME)
endif
endif

LIBNAME = lib$(TARGET).a

ifdef FLTK
  TOOLKIT_FILES = $(FLTK_CPPFILES)
endif

INCL_DEP = $(INCL) $(FLTK_H) $(ALL_SOURCEFILES)
ALL_INCL = $(INCL) $(FLTK_H) 

CXXFLAGS_=$(OPTS) $(ALL_INCL) $(CXXFLAGS)
CFLAG_   = $(OPTS) $(ALL_INCL) $(CFLAGS)

LDLIBS_ = -L./ $(I18N_LIB) $(FLTK_LIBS) $(LDFLAGS) \
	-L/lib \
	$(DBG_LIBS)

CPP_HEADERS = \
	threads.h

COMMON_CXXFILES = \
	$(TARGET)_example.cxx

LINGUAS := \
	$(shell ls po/*.po 2> /dev/null | sed -n 's/\.po//p' | sed -n 's%po\/%%p')
LING := \
	$(shell ls po/*.po 2> /dev/null)

COMMON_CFILES = 
CFILES = \
	$(COMMON_CFILES)
CPPFILES = \
	$(COMMON_CPPFILES) \
	$(TOOLKIT_FILES)
CXXFILES = \
	$(COMMON_CXXFILES)
TEST_CPP =
TEST_H = \
	$(TARGET)_example.h

ALL_SOURCEFILES = \
	$(COMMON_CFILES) \
	$(COMMON_CPPFILES) \
	$(FLTK_CPPFILES) \
	$(I18N_CXXFILES) \
	$(CXXFILES) \
	$(TEST_CPP)

ALL_HEADERFILES = \
	$(CPP_HEADERS) \
	$(I18N_HEADERS) \
	$(TEST_H)

DOKU = \
	TODO \
	README \
	ChangeLog \
	COPYING \
	BUGS \
	AUTHORS \
	INSTALL

SOURCES = $(ALL_SOURCEFILES) $(ALL_HEADERFILES)
OBJECTS = $(CPPFILES:.cpp=.o) $(CXXFILES:.cxx=.o)
I18N_OBJECTS =  $(I18N_CXXFILES:.cxx=.o)
LINGUAS_SRC = $(LING:.po=.gmo)

POT_FILE = po/$(TARGET).pot


ALL_FILES =	$(SOURCES) \
	Makefile \
	Makefile.bsd \
	makefile.in \
	$(DOKU)

# get time stamp
C := $(shell date +%C)
MTIME   := $(shell date +%Y%m%e.%H%M%S | sed s/$C//)


all:	dynamic
	
BASE =  
	

dynamic:	$(BINTARGET)

$(BINTARGET):	$(BASE) $(OBJECTS) $(LINGUAS_SRC) $(I18N_OBJECTS) $(LIBNAME)
	echo Verknuepfen $@ ... $(OBJECTS)
	-$(CXX) $(CXXFLAGS_) $(LDFLAGS) -o $(BINTARGET) \
	$(OBJECTS) \
	$(LDLIBS_) $(LINK_LIB_PATH) $(LINK_SRC_PATH)
	$(REZ)


$(LIBNAME):	$(I18N_OBJECTS)
	echo Verknuepfen $@ ...
	$(COLLECT) $(LIBNAME) $(I18N_OBJECTS)
	$(RANLIB) $(LIBNAME)

strip: $(BINTARGET)
	echo strip $(BINTARGET)
	strip $(BINTARGET)

static_static:	$(OBJECTS)
	$(CXX) $(OPTS) -o $(BINTARGET) \
	$(OBJECTS) \
	$(LDLIBS_) -static -ljpeg -lpng -lX11 -lpthread -lz $(dl) \
	-lfreetype -lfontconfig -lXrender -lGLU -lXext -lexpat \
	-L/opt/local/lib
	$(REZ)

$(LINGUAS_SRC):	$(POT_FILE)
	echo Linguas ... $(@:.gmo=)
	for ling in $(@:.gmo=); do \
      lingua=`echo $${ling} | sed s%po\/%%g`; \
	  echo "update $${ling}.gmo ..."; \
	  test -f $${ling}.po \
        && ($(MSGFMT) -o $${ling}.gmo $${ling}.po; \
            if [ ! -d $${ling} ]; then \
              mkdir $${ling}; \
            fi; \
            if [ ! -d ./$${ling}/LC_MESSAGES ]; then \
              mkdir -v $${ling}/LC_MESSAGES; \
            fi; \
            test -L $${ling}/LC_MESSAGES/$(TARGET).mo \
              && $(RM) $${ling}/LC_MESSAGES/$(TARGET).mo \
              || echo $${ling}/LC_MESSAGES/$(TARGET).mo nicht gefunden; \
            ln -s ../../$${lingua}.gmo $$ling/LC_MESSAGES/$(TARGET).mo;) \
        || (echo $${ling}.po is not yet ready ... skipping) \
	done;

$(POT_FILE):	$(ALL_SOURCEFILES)
	test `which xgettext` && xgettext --force-po $(XGETTEXT_OPTIONS) \
	-d $(TARGET) \
	-o $(POT_FILE) \
	$(SOURCES) || echo -e "\c"
	for ling in $(LINGUAS); do \
            test `which $(MSGMERGE)` && \
                 $(MSGMERGE) $(MSGMERG_OPTS) po/$${ling}.po \
                 $(POT_FILE) || \
	        echo -e "\c"; \
	done;
	echo $(POT_FILE) fertig


clean:
	echo mache sauber $@ ...
	-$(RM) $(OBJECTS) $(I18N_OBJECTS) $(BINTARGET) \
	$(LIBNAME)
	-for ling in $(LINGUAS); do \
	  test -f po/$${ling}.gmo \
        && $(RM) po/$${ling}.gmo; \
	done;
	echo ... $@ fertig


# The extension to use for executables...
EXEEXT		= 

# Build commands and filename extensions...
.SUFFIXES:	.0 .1 .3 .c .cxx .h .fl .man .o .z $(EXEEXT)

.c.o:
	echo Compiling $< ...
	$(CC) $(CFLAGS_) -c $<

.cxx.o:
	echo Compiling $< ...
	$(CXX) $(CXXFLAGS_) -c $<

.cpp.o:
	echo Compiling $< ...
	$(CXX) $(CXXFLAGS_) -c $<

.fl.cxx:
	echo Expanding $< ...
	fluid -c $<

.po:
	echo Generating $@ ...
	msgfmt $<

tgz:
	make DESTDIR=Entwickeln copy_files
	tar cf - Entwickeln/ \
	| gzip > $(TARGET)_$(MTIME).tgz
	test -d Entwickeln && \
	test `pwd` != `(cd Entwickeln; pwd)` && \
	$(RM) -R Entwickeln
	test -d ../../Archiv && mv -v $(TARGET)_*.tgz ../../Archiv

targz:
	make DESTDIR=$(TARGET)_$(VERSION) copy_files
	tar cf - $(TARGET)_$(VERSION)/ \
	| gzip > $(TARGET)_$(MTIME).tgz
	test -d $(TARGET)_$(VERSION) && \
	test `pwd` != `(cd $(TARGET)_$(VERSION); pwd)` && \
	$(RM) -R $(TARGET)_$(VERSION) 
	test -d ../../Archiv && mv -v $(TARGET)_*.tgz ../../Archiv || echo no copy

copy_files:
	echo Copy all fl_i18n files ...
	mkdir $(DESTDIR)
	$(COPY) -R \
	$(ALL_FILES) \
	$(DESTDIR)
	mkdir $(DESTDIR)/po
	$(COPY) $(POT_FILE) $(DESTDIR)/po
	for ling in $(LINGUAS); do \
	  test -f po/$${ling}.po \
        && $(COPY) po/$${ling}.po $(DESTDIR)/po/; \
	done;


dist: $(BASE) targz
	test -f && $(COPY) ../../Archiv/$(TARGET)_$(MTIME).tgz $(TARGET)_$(VERSION).tar.gz || $(COPY) $(TARGET)_$(MTIME).tgz $(TARGET)_$(VERSION).tar.gz

$(TARGET).spec:
	./configure


.PHONY: install uninstall
#.SILENT:


