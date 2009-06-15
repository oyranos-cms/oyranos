CPP = c++
CC = gcc
RM = rm -v

TARGET1 = image_display/image_display
TARGET2 = xcmsevents/xcmsevents
TARGET3 = colour_desktop/libcolour_desktop.so
TARGET4 = image2pdf/image2pdf
TARGET5 = dummy_device/liboyranos_dDev_cmm_module.so
TARGET6 = dummy_filter/liboyranos_dfil_cmm_module.so
TARGETS = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6)
OY_CFLAGS := $(shell (oyranos-config --cflags))
OY_LDFLAGS := $(shell (oyranos-config --ldflags))
FLTK_CFLAGS := $(shell (fltk-config --cflags))
FLTK_LDFLAGS := $(shell (fltk-config --ldflags))
PKG_CONFIG_CFLAGS := $(shell (pkg-config --cflags xfixes x11))
PKG_CONFIG_LIBS := $(shell (pkg-config --libs xfixes x11))
TARGET3_FLAGS := $(shell (pkg-config --cflags --libs compiz oyranos))
TARGET4_FLAGS := $(shell (pkg-config --cflags --libs cairo lcms oyranos))
LDFLAGS = $(LIBS)
DEBUG = -Wall -g -pedantic -O0
CFLAGS = $(OY_CFLAGS) $(INCLUDE) $(DEBUG)


UNAME := $(shell (uname -s))
ifeq ($(strip $(UNAME)),Linux)
X11 = -DHAVE_X11
endif

.SILENT:

all:	colour_desktop $(TARGETS)
	echo ... all done

colour_desktop:
	mkdir -p colour_desktop dummy_device image2pdf image_display xcmsevents
	

$(TARGET1):	$(SRCDIR)$(TARGET1).cpp
	echo Compiling $@ ...
	-$(CPP) -DTARGET=\"$(TARGET)\" $(CFLAGS) $(FLTK_CFLAGS) $(X11) \
	$(LDFLAGS) $(OY_LDFLAGS) $(FLTK_LDFLAGS) $(PKG_CONFIG_CFLAGS) $(PKG_CONFIG_LIBS) \
	$< -o $@
	fltk-config --post $@

$(TARGET2):	$(SRCDIR)$(TARGET2).c
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $(LDFLAGS) $(OY_LDFLAGS) $(PKG_CONFIG_CFLAGS) \
	$(PKG_CONFIG_LIBS) -lXcolor -lXmu $< -o $@

$(TARGET3):	$(SRCDIR)colour_desktop/colour_desktop.c
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -std=c99 -fPIC -shared $(LDFLAGS) $(OY_LDFLAGS) \
	$(TARGET3_FLAGS) -lXcolor $< -o $@

$(TARGET4):	$(SRCDIR)$(TARGET4).c
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) $(TARGET4_FLAGS) $(LDFLAGS) -lXcolor $< -o $@

$(TARGET5):	$(SRCDIR)dummy_device/oyranos_cmm_dDev.c
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -shared $(LDFLAGS) $(OY_LDFLAGS) $< -o $@

$(TARGET6):	$(SRCDIR)dummy_filter/oyranos_cmm_dFil.c
	echo Compiling $@ ...
	-$(CC) $(CFLAGS) -shared $(LDFLAGS) $(OY_LDFLAGS) $< -o $@

clean:
	echo cleaning ...
	$(RM) $(TARGETS)
	echo ... clean done

install:	$(TARGET3)
	mkdir -p $(HOME)/.compiz/plugins
	install -v $(TARGET3) $(HOME)/.compiz/plugins/libcolour_desktop.so

