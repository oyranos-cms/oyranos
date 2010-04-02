INCL    = $(INCLUDE) -I../ -I./ $(DEBUG)
CC      = gcc
DEBUG  = -Wall -g -O0
LDFLAGS_ = $(LDFLAGS) -L../fl_i18n
OY_FLAGS := $(shell pkg-config oyranos libxml-2.0 --libs --cflags)
FLTK_FLAGS := $(shell fltk-config --ldflags --cxxflags)
all:	oyranos-xforms oyranos-xforms-fltk
	echo ... done


oyranos-xforms:	$(SRCDIR)oyranos_xforms.c
	echo "Compiling $(SRCDIR)$< + Linking $@ ..."
	gcc -Wall -g $(OY_FLAGS) $(CFLAGS) $(INCL)  $(SRCDIR)oyranos_xforms.c -o oyranos-xforms -pedantic

oyranos-xforms-fltk:	$(SRCDIR)oyranos_xforms_fltk.cxx $(SRCDIR)oyranos_forms_fltk.cxx $(SRCDIR)oyranos_widgets_fltk.cxx $(SRCDIR)oyranos_widgets_fltk.h
	echo "Compiling $(SRCDIR)$< + Linking $@ ..."
	c++ $(LDFLAGS_) -Wall -g $(OY_FLAGS) $(CFLAGS) $(INCL)  $(FLTK_FLAGS) $(SRCDIR)oyranos_xforms_fltk.cxx -o oyranos-xforms-fltk -pedantic -loyforms_fltk -lfl_i18n


# Build commands and filename extensions...
.SUFFIXES:	.c .cxx .h .fl .o .po

.c.o:
	echo Compiling $(SRCDIR)$< ...
	$(CC) -I. $(CFLAGS) $(INCL) -c -o $@ $(SRCDIR)$<


