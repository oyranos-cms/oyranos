#CXX = c++
#CC = gcc
#RM = rm -v

TARGET1 = image_display/image_display
TARGET2 = xcmsevents/xcmsevents
TARGET3 = colour_desktop/libcolour_desktop.so
TARGET4 = image2pdf/image2pdf
TARGET5 = dummy_device/liboyranos_dDev_cmm_module.so
TARGET6 = dummy_filter/liboyranos_dFil_cmm_module.so
TARGET7 = oymd5icc/oymd5icc
TARGETS = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) \
	$(TARGET7)
OY_CFLAGS := $(shell (oyranos-config --cflags))
OY_LDFLAGS := $(shell (oyranos-config --ldflags))
FLTK_CFLAGS := $(shell (fltk-config --cflags))
FLTK_LDFLAGS := $(shell (fltk-config --ldflags))
PKG_CONFIG_CFLAGS := $(shell (pkg-config --cflags xfixes x11))
PKG_CONFIG_LIBS := $(shell (pkg-config --libs xfixes x11))
TARGET2_FLAGS = ../modules/devices/oyranos_monitor.o \
		../modules/devices/oyranos_edid_parse.o
TARGET3_FLAGS := $(shell (pkg-config --cflags --libs compiz oyranos))
TARGET4_FLAGS := $(shell (pkg-config --cflags --libs cairo lcms oyranos))
LDFLAGS = $(LIBS)
#DEBUG = -Wall -g -pedantic -O0
CFLAGS = $(OY_CFLAGS) $(INCLUDE) $(DEBUG)

CAIRO := $(shell (pkg-config --modversion cairo-pdf))
ifndef CAIRO
  TARGET4 =
endif
COMPIZ := $(shell (pkg-config --modversion compiz))
ifndef COMPIZ
  TARGET3 =
endif
UNAME := $(shell (uname -s))
ifeq ($(strip $(UNAME)),Linux)
X11 = -DHAVE_X11
endif

.SILENT:

all:	$(TARGETS)
	echo ... all done

	

$(TARGET1):	$(SRCDIR)$(TARGET1).cpp
	mkdir -p image_display
	echo Compiling $@ ...
	-$(CXX) -DTARGET=\"$(TARGET)\" $(CFLAGS) $(FLTK_CFLAGS) $(X11) \
	$(LDFLAGS) $(OY_LDFLAGS) $(FLTK_LDFLAGS) $(PKG_CONFIG_CFLAGS) \
	$(PKG_CONFIG_LIBS) -DUSE_GETTEXT=1 \
	$< -o $@ ../fl_i18n/libfl_i18n.a
	fltk-config --post $@

$(TARGET2):	$(SRCDIR)$(TARGET2).c
	mkdir -p xcmsevents
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $(LDFLAGS) $(OY_LDFLAGS) $(PKG_CONFIG_CFLAGS) \
	$(PKG_CONFIG_LIBS) -lXcolor -lXmu -lXinerama -lXrandr -lXxf86vm \
	$(TARGET2_FLAGS) $< -o $@

$(TARGET3):	$(SRCDIR)colour_desktop/colour_desktop.c
	mkdir -p colour_desktop
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -std=c99 -fPIC -shared $(LDFLAGS) $(OY_LDFLAGS) \
	$(TARGET3_FLAGS) -lXcolor $< -o $@

$(TARGET4):	$(SRCDIR)$(TARGET4).c
	mkdir -p image2pdf
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $(TARGET4_FLAGS) $(LDFLAGS) -lXcolor $< -o $@

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
	-$(CC) $(CFLAGS) $(LDFLAGS) $(OY_LDFLAGS) $< -o $@

clean:
	echo cleaning ...
	$(RM) $(TARGETS)
	echo ... clean done

install:	$(TARGET3)
	mkdir -p $(HOME)/.compiz/plugins
	install -v $(TARGET3) $(HOME)/.compiz/plugins/libcolour_desktop.so

