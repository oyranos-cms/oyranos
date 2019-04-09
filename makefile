prefix=/usr

CC = cc
DEBUG= -Wall -Wextra -g
INCL = -I$(prefix)/include -I./
LDFLAGS = -L$(prefix)/lib64 -lm
TARGET = oyjl-test-core
CFLAGS=$(DEBUG) $(INCL) -fPIC
SOURCES = \
	oyjl_args.c \
	oyjl_core.c \
	oyjl_tree.c
OBJECTS = $(SOURCES:.c=.o)
YSOURCES = oyjl_yajl.c $(SOURCES)
YOBJECTS = $(YSOURCES:.c=.o)
TSOURCES = test-core.c
TOBJECTS = $(TSOURCES:.c=.o)
OSOURCES = oyjl.c
OOBJECTS = $(OSOURCES:.c=.o)

all:	$(TARGET) oyjl jsontoyaml yamltojson

$(TARGET):	$(OBJECTS) $(TOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TOBJECTS) $(OBJECTS) $(LDFLAGS) -lyajl

check:
	./oyjl-test-core

oyjl: $(YOBJECTS) $(OOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OOBJECTS) $(YOBJECTS) $(LDFLAGS) -lyajl -lxml2 -lyaml

jsontoyaml: $(YOBJECTS) $(OOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OOBJECTS) $(YOBJECTS) $(LDFLAGS) -lyajl -lxml2 -lyaml

yamltojson: yaml2json.o $(YOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(YOBJECTS) $< $(LDFLAGS) -lyajl -lxml2 -lyaml

clean:
	$(RM) $(OBJECTS) $(TOBJECTS) $(OOBJECTS) $(YOBJECTS) jsontoyaml yaml2json.o yamltojson $(TARGET) oyjl

.SUFFIXES: .c.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
