edid-parse:	edid_parse.c oyranos_edid_parse.c
	cc -Wall -g -I../.. edid_parse.c -o edid-parse oyranos_edid_parse.o -lm
check:	edid-parse
	oyranos-monitor -f edid -o edid.bin
	edid-parse edid.bin
