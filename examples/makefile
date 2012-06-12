#CXX = c++
#CC = gcc
#RM = rm -v
#MOC = moc

#.SILENT:

TARGET1 = image_display/image_display
TARGET2 = 
TARGET4 = image2pdf/image2pdf
TARGET5 = dummy_device/liboyranos_dDev_cmm_module.so
TARGET6 = dummy_filter/liboyranos_dFil_cmm_module.so
TARGET7 = oymd5icc/oymd5icc
TARGET8 = xcmsevents/qcmsevents
TARGET9 = libraw/oyranos-file
TARGETS = $(TARGET1) $(TARGET2) $(TARGET4) $(TARGET5) $(TARGET6) \
	$(TARGET7) $(TARGET8) $(TARGET9) ../oyranos_logo.ppm
OY_CFLAGS := $(shell (pkg-config --cflags oyranos))
OY_LDFLAGS := $(shell (pkg-config --libs oyranos))
FLTK_CFLAGS := $(shell (fltk-config --cflags --use-gl | sed 's/-O.//g'))
FLTK_LDFLAGS := $(shell (fltk-config --ldflags --use-gl)) -pthread # FLTK on Fedora13 misses libpthread.so
PKG_CONFIG_CFLAGS := $(shell (pkg-config --cflags xfixes x11))
PKG_CONFIG_LIBS := $(shell (pkg-config --libs xfixes x11))
XCM_CFLAGS := $(shell (pkg-config --cflags xcm))
XCM_LDFLAGS := $(shell (pkg-config --libs xcm))
TARGET2_FLAGS = ../modules/devices/oyranos_monitor.o \
		../modules/devices/oyranos_edid_parse.o $(XCM_CFLAGS)
TARGET4_FLAGS := $(shell (pkg-config --cflags --libs cairo oyranos))
LDFLAGS = $(LIBS)
DEBUG = -Wall -g -pedantic -O0
CXXFLAGS_ = $(CXXFLAGS) $(OY_CFLAGS) $(INCLUDE) $(DEBUG) -std=c++98 $(XCM_CFLAGS)
CFLAGS_ = $(CFLAGS) -std=c99 -D__USE_POSIX2=1 $(OY_CFLAGS) $(INCLUDE) $(DEBUG) \
	$(X11) $(XCM_CFLAGS)

UNAME := $(shell (uname -s))

MOC_QT4:= $(shell (which moc-qt4 2>&1 | grep "no moc" | wc -l))
ifeq ($(MOC_QT4),0)
MOC = moc-qt4
else
MOC = moc
endif

ifeq ($(strip $(UNAME)),Linux)
# Qt needs somehow -O2 (Why???)
QT_CFLAGS := $(shell pkg-config --cflags QtGui xcm) -O2 -std=c++0x
INCL    := $(shell pkg-config --cflags-only-I QtGui)
TARGET8_LDFLAGS := $(LDFLAGS)  -L./xcmsevents $(shell pkg-config --libs QtGui x11 xmu xfixes xinerama xrandr xxf86vm oyranos xcm) -lQtXml -lQtCore
TARGET8_SOURCES = $(TARGET8).cpp
TARGET8_GENERATED_SOURCES = \
	xcmsevents/moc_qcmsevents.cpp \
	xcmsevents/qrc_qcmsevents.cpp
TARGET8_OBJECTS = $(TARGET8_SOURCES:.cpp=.o) $(TARGET8_GENERATED_SOURCES:.cpp=.o)
endif


CAIRO := $(shell (pkg-config --modversion cairo-pdf))
ifndef CAIRO
  TARGET4 =
endif
ifeq ($(strip $(UNAME)),Linux)
X11 = -DHAVE_X11
endif

all:
	echo Building all with:
	echo "INCLUDE=$$INCLUDE"
	echo "LIBS=$$LIBS"
	echo "PKG_CONFIG_PATH=$$PKG_CONFIG_PATH"
	echo "CFLAGS=$(CFLAGS)"
	echo "PATH=$$PATH"
	make $(TARGETS)
	echo ... all done

	

$(TARGET1):	$(SRCDIR)$(TARGET1).cpp
	mkdir -p image_display
	echo Compiling $@ ...
	-$(CXX) -DTARGET=\"$(TARGET)\" $(CXXFLAGS_) $(FLTK_CFLAGS) $< \
	$(X11) $(LDFLAGS) $(OY_LDFLAGS) \
	$(intl) ../fl_i18n/libfl_i18n.a  $(FLTK_LDFLAGS) $(PKG_CONFIG_CFLAGS) \
	$(PKG_CONFIG_LIBS) -DUSE_GETTEXT=1 \
	-o $@
	-fltk-config --post $@

$(TARGET2):	$(SRCDIR)$(TARGET2).c
	mkdir -p xcmsevents
	echo Compiling $@ ...
	-$(CC) $(CFLAGS_) $< $(TARGET2_FLAGS) $(LDFLAGS) $(OY_LDFLAGS) \
	$(PKG_CONFIG_CFLAGS) $(PKG_CONFIG_LIBS) \
	 $(XCM_LDFLAGS) -lXmu -lXinerama -lXrandr -lXxf86vm -lm \
	-DTARGET=\"xcmsevents\" -o $@

$(TARGET4):	$(SRCDIR)$(TARGET4).c
	mkdir -p image2pdf
	echo Compiling $@ ...
	-$(CC) $(CFLAGS_) $< $(TARGET4_FLAGS) $(LDFLAGS) $(XCM_LDFLAGS) \
	$(OY_LDFLAGS) -o $@

$(TARGET5):	$(SRCDIR)dummy_device/oyranos_cmm_dDev.c
	mkdir -p dummy_device
	echo Compiling $@ ...
	-$(CC) $(CFLAGS_) -shared $(LDFLAGS) $(OY_LDFLAGS) -fPIC $< -o $@

$(TARGET6):	$(SRCDIR)dummy_filter/oyranos_cmm_dFil.c
	mkdir -p dummy_filter
	echo Compiling $@ ...
	-$(CC) $(CFLAGS_) -shared $(LDFLAGS) $(OY_LDFLAGS) -fPIC $< -o $@

$(TARGET7):	$(SRCDIR)oymd5icc/oymd5icc.c
	mkdir -p oymd5icc
	echo Compiling $@ ...
	-$(CC) $(CFLAGS_) $< $(LDFLAGS) $(OY_LDFLAGS) -o $@

$(TARGET8):	$(TARGET8_OBJECTS)
	mkdir -p xcmsevents
	echo "	LINK:	$(TARGET8) $(TARGET8_SOURCES) ..."
	-$(CXX) -Wall -g -o $(TARGET8) $(TARGET8_OBJECTS) $(TARGET8_LDFLAGS)

$(TARGET9):	$(SRCDIR)libraw/oyranos_file.cpp
	mkdir -p libraw
	echo Compiling $@ ...
	-$(CXX) $(CXXFLAGS_) $< $(LDFLAGS) $(OY_LDFLAGS) -o $@

../oyranos_logo.ppm:	$(SRCDIR)../oyranos_logo.png
	-cat $(SRCDIR)../oyranos_logo.png | pngtopnm > $@

clean:
	echo cleaning ...
	$(RM) $(TARGETS)
	$(RM) $(TARGET8_OBJECTS) $(TARGET8_GENERATED_SOURCES)
	echo ... clean done

xcmsevents/qrc_qcmsevents.cpp:
	echo "	RCC:	$@ ..."
	-rcc $(SRCDIR)$(TARGET8).qrc -o $@

xcmsevents/moc_qcmsevents.cpp:	$(SRCDIR)$(TARGET8).h
	echo "	MOC:	$@ ..."
	-$(MOC) $(SRCDIR)$(TARGET8).h -o $@

$(TARGET8).o:	$(SRCDIR)$(TARGET8).cpp
	echo "	C++:	$@ ..."
	-$(CXX) -I. $(CXXFLAGS_) $(QT_CFLAGS) $(INCL) -c -o $@ $<

# Build commands and filename extensions...
.SUFFIXES:	.c .cxx .cpp .h .o


.c.o:	$(HEADERS)
	echo "	CC:	$< ..."
	-$(CC) -I. $(CFLAGS_) $(INCL) -c -o $@ $<

.cxx.o:	$(HEADERS)
	echo "	C++:	$< ..."
	-$(CXX) -I. $(CXXFLAGS_) $(QT_CFLAGS) $(INCL) -c -o $@ $<

.cpp.o:	$(HEADERS)
	echo "	C++:	$< ..."
	-$(CXX) -I. $(CXXFLAGS_) $(INCL) -c -o $@ $<


