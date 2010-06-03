#CXX = c++
#CC = gcc
#RM = rm -v
#MOC = moc

#.SILENT:

TARGET1 = image_display/image_display
TARGET2 = xcmsevents/xcmsevents
TARGET3 = colour_desktop/libcolour_desktop.so
TARGET4 = image2pdf/image2pdf
TARGET5 = dummy_device/liboyranos_dDev_cmm_module.so
TARGET6 = dummy_filter/liboyranos_dFil_cmm_module.so
TARGET7 = oymd5icc/oymd5icc
TARGET8 = xcmsevents/qcmsevents
TARGETS = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) \
	$(TARGET7) $(TARGET8) ../oyranos_logo.ppm
OY_CFLAGS := $(shell (oyranos-config --cflags))
OY_LDFLAGS := $(shell (oyranos-config --ldflags))
FLTK_CFLAGS := $(shell (fltk-config --cflags | sed 's/-0.//g'))
FLTK_LDFLAGS := $(shell (fltk-config --ldflags)) -pthread # FLTK on Fedora13 misses libpthread.so
PKG_CONFIG_CFLAGS := $(shell (pkg-config --cflags xfixes x11))
PKG_CONFIG_LIBS := $(shell (pkg-config --libs xfixes x11))
TARGET2_FLAGS = ../modules/devices/oyranos_monitor.o \
		../modules/devices/oyranos_edid_parse.o
TARGET3_FLAGS := $(shell (pkg-config --cflags --libs compiz oyranos))
TARGET4_FLAGS := $(shell (pkg-config --cflags --libs cairo lcms oyranos))
LDFLAGS = $(LIBS)
DEBUG = -Wall -g -pedantic -O0
CXXFLAGS = $(OY_CFLAGS) $(INCLUDE) $(DEBUG) -std=c++0x
CFLAGS = -std=c99 -D__USE_POSIX2=1 $(OY_CFLAGS) $(INCLUDE) $(DEBUG) $(X11)

UNAME := $(shell (uname -s))

MOC_QT4:= $(shell (which moc-qt4 2>&1 | grep "no moc" | wc -l))
ifeq ($(MOC_QT4),0)
MOC = moc-qt4
else
MOC = moc
endif

ifeq ($(strip $(UNAME)),Linux)
# Qt needs somehow -O2 (Why???)
QT_CFLAGS := $(shell pkg-config --cflags QtGui) -O2 -std=c++0x
INCL    := $(shell pkg-config --cflags-only-I QtGui)
TARGET8_LDFLAGS :=  -L./xcmsevents -loyranosedid -lXcolor $(shell pkg-config --libs QtGui x11 xmu xfixes xinerama xrandr xxf86vm oyranos) -lQtXml -lQtCore
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
COMPIZ := $(shell (pkg-config --modversion compiz))
ifndef COMPIZ
  TARGET3 =
endif
ifeq ($(strip $(UNAME)),Linux)
X11 = -DHAVE_X11
endif

all:
	echo Building all with:
	echo "INCLUDE=$$INCLUDE"
	echo "LIBS=$$LIBS"
	echo "PKG_CONFIG_PATH=$$PKG_CONFIG_PATH"
	make $(TARGETS)
	echo ... all done

	

$(TARGET1):	$(SRCDIR)$(TARGET1).cpp
	mkdir -p image_display
	echo Compiling $@ ...
	-$(CXX) -DTARGET=\"$(TARGET)\" $(CXXFLAGS) $(FLTK_CFLAGS) $< \
	$(X11) $(LDFLAGS) $(OY_LDFLAGS) \
	../fl_i18n/libfl_i18n.a  $(FLTK_LDFLAGS) $(PKG_CONFIG_CFLAGS) \
	$(PKG_CONFIG_LIBS) -DUSE_GETTEXT=1 \
	-o $@
	fltk-config --post $@

$(TARGET2):	$(SRCDIR)$(TARGET2).c
	mkdir -p xcmsevents
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $< $(TARGET2_FLAGS) $(LDFLAGS) $(OY_LDFLAGS) \
	$(PKG_CONFIG_CFLAGS) $(PKG_CONFIG_LIBS) \
	-lXcolor -lXmu -lXinerama -lXrandr -lXxf86vm -lm \
	-DTARGET=\"xcmsevents\" -o $@

$(TARGET3):	$(SRCDIR)colour_desktop/colour_desktop.c
	mkdir -p colour_desktop
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -std=c99 -fPIC -shared $< $(LDFLAGS) $(OY_LDFLAGS) \
	$(TARGET3_FLAGS) -lXcolor -o $@

$(TARGET4):	$(SRCDIR)$(TARGET4).c
	mkdir -p image2pdf
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $< $(TARGET4_FLAGS) $(LDFLAGS) -lXcolor $(OY_LDFLAGS) \
	-o $@

$(TARGET5):	$(SRCDIR)dummy_device/oyranos_cmm_dDev.c
	mkdir -p dummy_device
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -shared $(LDFLAGS) $(OY_LDFLAGS) -fPIC $< -o $@

$(TARGET6):	$(SRCDIR)dummy_filter/oyranos_cmm_dFil.c
	mkdir -p dummy_filter
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -shared $(LDFLAGS) $(OY_LDFLAGS) -fPIC $< -o $@

$(TARGET7):	$(SRCDIR)oymd5icc/oymd5icc.c
	mkdir -p oymd5icc
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $< $(LDFLAGS) $(OY_LDFLAGS) -o $@

$(TARGET8):	$(TARGET8_OBJECTS) xcmsevents/liboyranosedid.a
	echo "	LINK:	$(TARGET8) $(TARGET8_SOURCES) ..."
	-$(CXX) -Wall -g -o $(TARGET8) $(TARGET8_OBJECTS) $(TARGET8_LDFLAGS)

../oyranos_logo.ppm:	$(SRCDIR)../oyranos_logo.png
	-cat $(SRCDIR)../oyranos_logo.png | pngtopnm > $@

clean:
	echo cleaning ...
	$(RM) $(TARGETS)
	$(RM) $(TARGET8_OBJECTS) $(TARGET8_GENERATED_SOURCES)
	echo ... clean done

install:	$(TARGET3)
	mkdir -p $(HOME)/.compiz/plugins
	mkdir -p $(HOME)/.compiz/metadata
	mkdir -p $(HOME)/.local/share/icons
	-install -v $(TARGET3) $(HOME)/.compiz/plugins/libcolour_desktop.so
	-install -v $(SRCDIR)colour_desktop/colour_desktop.xml $(HOME)/.compiz/metadata/
	-install -v $(SRCDIR)colour_desktop/plugin-colour_desktop.svg $(HOME)/.local/share/icons/


xcmsevents/qrc_qcmsevents.cpp:
	echo "	RCC:	$@ ..."
	-rcc $(SRCDIR)$(TARGET8).qrc -o $@

xcmsevents/liboyranosedid.a:	../oyranos_version.h
	echo "	AR:	$@ ..."
	-ar cru $@ ../modules/devices/oyranos_edid_parse.o ../modules/devices/oyranos_monitor.o

xcmsevents/moc_qcmsevents.cpp:	$(SRCDIR)$(TARGET8).h
	echo "	MOC:	$@ ..."
	-$(MOC) $(SRCDIR)$(TARGET8).h -o $@

$(TARGET8).o:	$(SRCDIR)$(TARGET8).cpp
	echo "	C++:	$@ ..."
	-$(CXX) -I. $(CXXFLAGS) $(QT_CFLAGS) $(INCL) -c -o $@ $<

# Build commands and filename extensions...
.SUFFIXES:	.c .cxx .cpp .h .o


.c.o:	$(HEADERS)
	echo "	CC:	$< ..."
	-$(CC) -I. $(CFLAGS) $(INCL) -c -o $@ $<

.cxx.o:	$(HEADERS)
	echo "	C++:	$< ..."
	-$(CXX) -I. $(CXXFLAGS) $(QT_CFLAGS) $(INCL) -c -o $@ $<

.cpp.o:	$(HEADERS)
	echo "	C++:	$< ..."
	-$(CXX) -I. $(CXXFLAGS) $(INCL) -c -o $@ $<


