all:	oy_filter_node oyranos-xforms
	echo ... done

oy_filter_node:	oy_filter_node.c
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` oy_filter_node.c -o oy_filter_node
oyranos-xforms:	oyranos_xforms.c
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` -I../../Entwickeln -I../../build_10.3  oyranos_xforms.c -o oyranos-xforms -pedantic
