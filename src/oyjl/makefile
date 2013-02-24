prefix=/usr

CC = gcc
DEBUG= -Wall -g
INCL = -I$(prefix)/include -I../../yajl/
LDFLAGS = -L$(prefix)/lib64
TARGET = oyjl_tree_test
CFLAGS=$(DEBUG) $(INCL) -fPIC
SOURCES = \
	oyjl_tree.c \
	$(TARGET).c
OBJECTS = $(SOURCES:.c=.o)

all:	$(TARGET)

$(TARGET):	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS) -lyajl

check:
	./oyjl_tree_test
	./oyjl_tree_test org/freedesktop/openicc/device/[0]/[0]

clean:
	$(RM) $(OBJECTS) $(TARGET)

.SUFFIXES: .c.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
