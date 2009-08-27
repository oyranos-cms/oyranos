all:
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` oy_filter_node.c -o oy_filter_node
	gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` -I../../Entwickeln -I../../build_10.3  oyranos_xforms.c -o oyranos_xforms
