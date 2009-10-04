all:	oyranos-xforms
	echo ... done

oyranos-xforms:	oyranos_xforms.c
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` -I.. -I../../build_10.3  oyranos_xforms.c -o oyranos-xforms -pedantic
