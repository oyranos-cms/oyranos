prefix=/usr

CC = gcc
DEBUG= -Wall -Wextra -g
INCL = -I$(prefix)/include
LDFLAGS = -L$(prefix)/lib64
TARGET = oyjl_tree_test
CFLAGS=$(DEBUG) $(INCL) -fPIC
SOURCES = \
	oyjl_core.c \
	oyjl_tree.c
OBJECTS = $(SOURCES:.c=.o)
TSOURCES = $(TARGET).c
TOBJECTS = $(TSOURCES:.c=.o)
OSOURCES = oyjl.c
OOBJECTS = $(OSOURCES:.c=.o)

all:	$(TARGET) oyjl jsontoyaml yamltojson

$(TARGET):	$(OBJECTS) $(TOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TOBJECTS) $(OBJECTS) $(LDFLAGS) -lyajl

check:
	./oyjl_tree_test
	./oyjl_tree_test org/freedesktop/openicc/device/[0]/[0]

oyjl: $(OBJECTS) $(OOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OOBJECTS) $(OBJECTS) $(LDFLAGS) -lyajl

jsontoyaml: $(OBJECTS) $(OOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OOBJECTS) $(OBJECTS) $(LDFLAGS) -lyajl

yamltojson: yaml2json.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $< $(LDFLAGS) -lyajl -lyaml

clean:
	$(RM) $(OBJECTS) $(TOBJECTS) $(OOBJECTS) $(TARGET) oyjl

.SUFFIXES: .c.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
