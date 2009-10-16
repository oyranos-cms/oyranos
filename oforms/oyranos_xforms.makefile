all:	oyranos-xforms oyranos-xforms-fltk
	echo ... done

oyranos-xforms:	oyranos_xforms.c
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` -I../../Entwickeln -I../../build_10.3  oyranos_xforms.c -o oyranos-xforms -pedantic

oyranos-xforms-fltk:	oyranos_xforms_fltk.cxx oyranos_forms_fltk.cxx oyranos_widgets_fltk.cxx oyranos_widgets_fltk.h
	c++ $(LDFLAGS) -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` -I../../Entwickeln -I../../build_10.3  `fltk-config --ldflags --cxxflags` oyranos_xforms_fltk.cxx -o oyranos-xforms-fltk -pedantic -loyforms_fltk -lfl_i18n
