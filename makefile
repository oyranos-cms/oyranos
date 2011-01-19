CC      = gcc
CFLAGS  = -g -Wall -pedantic -DUSE_GETTEXT -fPIC
SRCDIR  = .
XML_INC := $(shell pkg-config --cflags libxml-2.0)
INCL    = -IAPI_generated$(OBJECTS_DIR)/ -Iinclude_core/ $(XML_INC)

OBJECTS_DIR = _object_core

SOURCES_OBJ_CORE = \
	API_generated$(OBJECTS_DIR)/oyBlob_s_.c \
	API_generated$(OBJECTS_DIR)/oyBlob_s.c \
	API_generated$(OBJECTS_DIR)/oyCMMptr_s_.c \
	API_generated$(OBJECTS_DIR)/oyCMMptr_s.c \
	API_generated$(OBJECTS_DIR)/oyHash_s_.c \
	API_generated$(OBJECTS_DIR)/oyHash_s.c \
	API_generated$(OBJECTS_DIR)/oyName_s_.c \
	API_generated$(OBJECTS_DIR)/oyName_s.c \
	API_generated$(OBJECTS_DIR)/oyObject_s_.c \
	API_generated$(OBJECTS_DIR)/oyObject_s.c \
	API_generated$(OBJECTS_DIR)/oyObserver_s_.c \
	API_generated$(OBJECTS_DIR)/oyObserver_s.c \
	API_generated$(OBJECTS_DIR)/oyOption_s_.c \
	API_generated$(OBJECTS_DIR)/oyOption_s.c \
	API_generated$(OBJECTS_DIR)/oyOptions_s_.c \
	API_generated$(OBJECTS_DIR)/oyOptions_s.c \
	API_generated$(OBJECTS_DIR)/oyStructList_s_.c \
	API_generated$(OBJECTS_DIR)/oyStructList_s.c \
	API_generated$(OBJECTS_DIR)/oyStruct_s.c \
	API_generated$(OBJECTS_DIR)/oyranos_object.c
HEADERS_OBJ_CORE = \
	API_generated$(OBJECTS_DIR)/oyBlob_s_.h \
	API_generated$(OBJECTS_DIR)/oyBlob_s.h \
	API_generated$(OBJECTS_DIR)/oyCMMptr_s_.h \
	API_generated$(OBJECTS_DIR)/oyCMMptr_s.h \
	API_generated$(OBJECTS_DIR)/oyHash_s_.h \
	API_generated$(OBJECTS_DIR)/oyHash_s.h \
	API_generated$(OBJECTS_DIR)/oyName_s_.h \
	API_generated$(OBJECTS_DIR)/oyName_s.h \
	API_generated$(OBJECTS_DIR)/oyObject_s_.h \
	API_generated$(OBJECTS_DIR)/oyObject_s.h \
	API_generated$(OBJECTS_DIR)/oyObserver_s_.h \
	API_generated$(OBJECTS_DIR)/oyObserver_s.h \
	API_generated$(OBJECTS_DIR)/oyOption_s_.h \
	API_generated$(OBJECTS_DIR)/oyOption_s.h \
	API_generated$(OBJECTS_DIR)/oyOptions_s_.h \
	API_generated$(OBJECTS_DIR)/oyOptions_s.h \
	API_generated$(OBJECTS_DIR)/oyranos_generic.h \
	API_generated$(OBJECTS_DIR)/oyranos_generic_internal.h \
	API_generated$(OBJECTS_DIR)/oyranos_module.h \
	API_generated$(OBJECTS_DIR)/oyranos_module_internal.h \
	API_generated$(OBJECTS_DIR)/oyranos_object.h \
	API_generated$(OBJECTS_DIR)/oyranos_object_internal.h \
	API_generated$(OBJECTS_DIR)/oyranos_profile.h \
	API_generated$(OBJECTS_DIR)/oyStructList_s_.h \
	API_generated$(OBJECTS_DIR)/oyStructList_s.h \
	API_generated$(OBJECTS_DIR)/oyStruct_s.h \
	API_generated$(OBJECTS_DIR)/oyTest.h

OBJECTS_OBJ_CORE = ${SOURCES_OBJ_CORE:.c=.o}
#.SILENT:

all:	generate_objects ${OBJECTS_OBJ_CORE}
	$(CC) -shared ${OBJECTS_OBJ_CORE} ../../oyranos/liboyranos_core.a \
	-o liboyranos$(OBJECTS_DIR).so

generate_objects:
	cd generator; ./oyAPIGenerator ../templates$(OBJECTS_DIR) ../sources$(OBJECTS_DIR) ../API_generated$(OBJECTS_DIR)

test:
	gcc -Wall -g `pkg-config --cflags oyranos` -IAPI_generated$(OBJECTS_DIR)/ object.c -o o -L./ -loyranos$(OBJECTS_DIR) -L/opt/local/lib64/ -loyranos_core -lxml2 -lm

# Build commands and filename extensions...
.SUFFIXES:	.c .h .o


.c.o:
	echo Compiling $(SRCDIR)/$< ...
	$(CC) -I. $(CFLAGS) $(INCL) -c -o $@ $(SRCDIR)/$<

clean:
	rm ${OBJECTS_OBJ_CORE}

