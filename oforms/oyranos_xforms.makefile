INCL    = $(INCLUDE) -I../ -I./ $(DEBUG)
CC      = gcc
DEBUG  = -Wall -g -O0
LDFLAGS_ = $$LDFLAGS -L../fl_i18n
OY_LIBS := $(shell pkg-config oyranos libxml-2.0 --libs --cflags)
OY_FLAGS = -L../ $(OY_LIBS)
FLTK_FLAGS := $(shell fltk-config --ldflags --cxxflags | sed s/-O2//) -lpthread

all:	oyranos-xforms oyranos-xforms-modules oyranos-xforms-fltk
	echo ... done

CXXFILES = \
	oyranos_xforms_fltk.cxx oyranos_forms_fltk.cxx oyranos_widgets_fltk.cxx 
CFILES = oyranos_xforms.c
OBJECTS = $(CXXFILES:.cxx=.o) $(CFILES:.c=.o)

oyranos-xforms:	$(SRCDIR)oyranos_xforms.c
	echo "Compiling $(SRCDIR)$< + Linking $@ ..."
	gcc -Wall -g $(CFLAGS) $(INCL)  $(SRCDIR)oyranos_xforms.c -o oyranos-xforms -pedantic $(OY_FLAGS) $(LDFLAGS_)

oyranos-xforms-modules:	$(SRCDIR)oyranos_xforms_modules.c
	echo "Compiling $(SRCDIR)$< + Linking $@ ..."
	gcc -Wall -g $(CFLAGS) $(INCL)  $< -o $@ -pedantic $(OY_FLAGS) $(LDFLAGS_)

oyranos-xforms-fltk:	$(SRCDIR)oyranos_xforms_fltk.cxx $(SRCDIR)oyranos_forms_fltk.cxx $(SRCDIR)oyranos_widgets_fltk.cxx $(SRCDIR)oyranos_widgets_fltk.h
	echo "Compiling $(SRCDIR)$< + Linking $@ ..."
	-c++ $(LDFLAGS_) -Wall -g $(CXXFLAGS) $(INCL)  $(SRCDIR)oyranos_xforms_fltk.cxx -o oyranos-xforms-fltk -pedantic -loyforms_fltk -lfl_i18n $(FLTK_FLAGS) $(OY_FLAGS)


# Build commands and filename extensions...
.SUFFIXES:	.c .cxx .h .fl .o .po

.c.o:
	echo Compiling $(SRCDIR)$< ...
	$(CC) -I. $(CFLAGS) $(INCL) -c -o $@ $(SRCDIR)$<

clean:
	$(RM) oyranos-xforms oyranos-xforms-fltk 
