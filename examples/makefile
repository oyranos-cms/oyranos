CPP = c++
CC = gcc
RM = rm -v

TARGET1 = image_display/image_display
TARGET2 = xcmsevents/xcmsevents
TARGET3 = colour_desktop/libcolour_desktop.so
TARGET4 = image2pdf/image2pdf
TARGET5 = dummy_device/liboyranos_dDev_cmm_module.so
TARGETS = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5)
OY_CFLAGS := $(shell (oyranos-config --cflags))
OY_LDFLAGS := $(shell (oyranos-config --ldflags))
FLTK_CFLAGS := $(shell (fltk-config --cflags))
FLTK_LDFLAGS := $(shell (fltk-config --ldflags))
PKG_CONFIG_CFLAGS := $(shell (pkg-config --cflags xfixes x11))
PKG_CONFIG_LIBS := $(shell (pkg-config --libs xfixes x11))
TARGET3_FLAGS := $(shell (pkg-config --cflags --libs compiz lcms oyranos))
TARGET4_FLAGS := $(shell (pkg-config --cflags --libs cairo lcms oyranos))
LDFLAGS = $(OY_LDFLAGS) $(FLTK_LDFLAGS)
DEBUG = -Wall -g -pedantic -O0
CFLAGS = $(OY_CFLAGS) $(FLTK_CFLAGS) \
	 $(DEBUG) -DTARGET=\"$(TARGET)\"


UNAME := $(shell (uname -s))
ifeq ($(strip $(UNAME)),Linux)
X11 = -DHAVE_X11
endif

.SILENT:

all:	$(TARGETS)
	echo ... all done

$(TARGET1):	$(TARGET1).cpp
	echo Compiling $@ ...
	-$(CPP) $(CFLAGS) $(LDFLAGS) $(PKG_CONFIG_CFLAGS) $(PKG_CONFIG_LIBS) \
	$< -o $@
	fltk-config --post $@

$(TARGET2):	$(TARGET2).c
	echo Compiling $@ ...
	-$(CC) $(DEBUG) $(OY_CFLAGS) $(OY_LDFLAGS) $(PKG_CONFIG_CFLAGS) \
	$(PKG_CONFIG_LIBS) -lXcolor $< -o $@

$(TARGET3):	colour_desktop/colour_desktop.c
	echo Compiling $@ ...
	-$(CC) $(DEBUG) -std=c99 -fPIC -shared $(OY_CFLAGS) $(OY_LDFLAGS) \
	$(TARGET3_FLAGS) -lXcolor $< -o $@

$(TARGET4):	$(TARGET4).c
	echo Compiling $@ ...
	-$(CC) $(DEBUG) $(TARGET4_FLAGS) -lXcolor $< -o $@

$(TARGET5):	dummy_device/oyranos_cmm_dDev.c
	echo Compiling $@ ...
	-$(CC) $(DEBUG) -shared $(OY_CFLAGS) $(OY_LDFLAGS) $< -o $@

clean:
	echo cleaning ...
	$(RM) $(TARGETS)
	echo ... clean done

install:	$(TARGET3)
	mkdir -p $(HOME)/.compiz/plugins
	install -v $(TARGET3) $(HOME)/.compiz/plugins/libcolour_desktop.so

