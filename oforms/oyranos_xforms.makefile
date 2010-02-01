INCL    = -I../../Entwickeln/ -I../ -I./ \
	  `pkg-config oyranos libxml-2.0 --cflags` $(DEBUG)
CC      = gcc
DEBUG  = -Wall -g
SRCDIR  = .
LDFLAGS_ = $(LDFLAGS) -L../fl_i18n

all:	oyranos-xforms oyranos-xforms-fltk
	echo ... done


oyranos-xforms:	oyranos_xforms.c
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` $(CFLAGS) $(INCL)  oyranos_xforms.c -o oyranos-xforms -pedantic

oyranos-xforms-fltk:	oyranos_xforms_fltk.cxx oyranos_forms_fltk.cxx oyranos_widgets_fltk.cxx oyranos_widgets_fltk.h
	c++ $(LDFLAGS_) -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` $(CFLAGS) $(INCL)  `fltk-config --ldflags --cxxflags` oyranos_xforms_fltk.cxx -o oyranos-xforms-fltk -pedantic -loyforms_fltk -lfl_i18n


# Build commands and filename extensions...
.SUFFIXES:	.c .cxx .h .fl .o .po

.c.o:
	echo Compiling $(SRCDIR)/$< ...
	$(CC) -I. $(CFLAGS) $(INCL) -c -o $@ $(SRCDIR)/$<


