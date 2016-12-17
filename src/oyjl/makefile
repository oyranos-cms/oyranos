prefix=/usr

CC = gcc
DEBUG= -Wall -g
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

all:	$(TARGET)

$(TARGET):	$(OBJECTS) $(TOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TOBJECTS) $(OBJECTS) $(LDFLAGS) -lyajl

check:
	./oyjl_tree_test
	./oyjl_tree_test org/freedesktop/openicc/device/[0]/[0]

oyjl:	all $(OOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OOBJECTS) $(OBJECTS) $(LDFLAGS) -lyajl

clean:
	$(RM) $(OBJECTS) $(TOBJECTS) $(OOBJECTS) $(TARGET) oyjl

.SUFFIXES: .c.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
