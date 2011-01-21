CC      = gcc
CFLAGS  = -g -Wall -pedantic -DUSE_GETTEXT -fPIC
SRCDIR  = .
OY_LIBS = -L../../oyranos -loyranos_core
XML_INC := $(shell pkg-config --cflags libxml-2.0)
INCL    = -IAPI_generated/ -Iinclude_core/ $(XML_INC)
.SILENT:

TARGET_OBJECT_CORE = _object_core
TARGET_ICC = _object_icc
TARGET_MODULE = _object

SOURCES_OBJ_CORE = \
	API_generated/oyBlob_s_.c \
	API_generated/oyBlob_s.c \
	API_generated/oyCMMptr_s_.c \
	API_generated/oyCMMptr_s.c \
	API_generated/oyHash_s_.c \
	API_generated/oyHash_s.c \
	API_generated/oyName_s_.c \
	API_generated/oyName_s.c \
	API_generated/oyObject_s_.c \
	API_generated/oyObject_s.c \
	API_generated/oyObserver_s_.c \
	API_generated/oyObserver_s.c \
	API_generated/oyOption_s_.c \
	API_generated/oyOption_s.c \
	API_generated/oyOptions_s_.c \
	API_generated/oyOptions_s.c \
	API_generated/oyStructList_s_.c \
	API_generated/oyStructList_s.c \
	API_generated/oyStruct_s.c \
	API_generated/oyranos_object.c
HEADERS_OBJ_CORE = \
	API_generated/oyBlob_s_.h \
	API_generated/oyBlob_s.h \
	API_generated/oyCMMptr_s_.h \
	API_generated/oyCMMptr_s.h \
	API_generated/oyHash_s_.h \
	API_generated/oyHash_s.h \
	API_generated/oyName_s_.h \
	API_generated/oyName_s.h \
	API_generated/oyObject_s_.h \
	API_generated/oyObject_s.h \
	API_generated/oyObserver_s_.h \
	API_generated/oyObserver_s.h \
	API_generated/oyOption_s_.h \
	API_generated/oyOption_s.h \
	API_generated/oyOptions_s_.h \
	API_generated/oyOptions_s.h \
	API_generated/oyranos_generic.h \
	API_generated/oyranos_generic_internal.h \
	API_generated/oyranos_module.h \
	API_generated/oyranos_module_internal.h \
	API_generated/oyranos_object.h \
	API_generated/oyranos_object_internal.h \
	API_generated/oyranos_profile.h \
	API_generated/oyStructList_s_.h \
	API_generated/oyStructList_s.h \
	API_generated/oyStruct_s.h \
	API_generated/oyTest.h

SOURCES_OBJ_ICC = \
	API_generated/oyProfile_s.c \
	API_generated/oyProfile_s.c \
	API_generated/oyProfiles_s_.c \
	API_generated/oyProfiles_s_.c \
	API_generated/oyProfileTag_s.c \
	API_generated/oyProfileTag_s_.c
HEADERS_OBJ_ICC = \
	API_generated/oyProfile_s.h \
	API_generated/oyProfile_s_.h \
	API_generated/oyProfiles_s.h \
	API_generated/oyProfiles_s_.h \
	API_generated/oyProfileTag_s.h \
	API_generated/oyProfileTag_s_.h

SOURCES_OBJ_MODULE = \
	API_generated/oyConfig_s.c \
	API_generated/oyConfig_s_.c \
	API_generated/oyConfigs_s.c \
	API_generated/oyConfigs_s_.c
HEADERS_OBJ_MODULE = \
	API_generated/oyConfig_s.h \
	API_generated/oyConfig_s_.h \
	API_generated/oyConfigs_s.h \
	API_generated/oyConfigs_s_.h

OBJECTS_OBJ_CORE = ${SOURCES_OBJ_CORE:.c=.o}
OBJECTS_OBJ_ICC = ${SOURCES_OBJ_ICC:.c=.o}
OBJECTS_OBJ_MODULE = ${SOURCES_OBJ_MODULE:.c=.o}

.PHONY: API_generated
all:	API_generated liboyranos$(TARGET_OBJECT_CORE).so liboyranos$(TARGET_MODULE).so
# liboyranos$(TARGET_ICC).so

liboyranos$(TARGET_OBJECT_CORE).so: $(HEADERS_OBJ_CORE) $(OBJECTS_OBJ_CORE)
	echo Linking $@ ...
	$(CC) -shared $(OBJECTS_OBJ_CORE) $(OY_LIBS) \
	-o liboyranos$(TARGET_OBJECT_CORE).so

liboyranos$(TARGET_ICC).so: $(OBJECTS_OBJ_ICC) liboyranos$(TARGET_OBJECT_CORE).so
	echo Linking $@ ...
	$(CC) -shared $(OBJECTS_OBJ_ICC) liboyranos$(TARGET_OBJECT_CORE).so \
	$(OY_LIBS) \
	-o liboyranos$(TARGET_ICC).so 

liboyranos$(TARGET_MODULE).so: $(OBJECTS_OBJ_MODULE) liboyranos$(TARGET_MODULE).so
	echo Linking $@ ...
	$(CC) -shared $(OBJECTS_OBJ_MODULE) liboyranos$(TARGET_OBJECT_CORE).so \
	$(OY_LIBS) \
	-o liboyranos$(TARGET_MODULE).so 

API_generated:	generator/oyAPIGenerator
	cd generator; ./oyAPIGenerator ../templates ../sources ../API_generated

generator/oyAPIGenerator:
	cd generator; cmake -DGrantlee_DIR="../../grantlee/install/dir" .; make

test:
	gcc -Wall -g `pkg-config --cflags oyranos` -IAPI_generated/ object.c -o o -L./ -loyranos$(TARGET_OBJECT_CORE) -L/opt/local/lib64/  -lxml2 -lm

# Build commands and filename extensions...
.SUFFIXES:	.c .h .o


.c.o:	$<
	echo Compiling $(SRCDIR)/$< ...
	$(CC) -I. $(CFLAGS) $(INCL) -c -o $@ $(SRCDIR)/$<

clean:
	rm $(OBJECTS_OBJ_CORE) $(OBJECTS_OBJ_ICC) liboyranos$(TARGET_OBJECT_CORE).so liboyranos$(TARGET_ICC).so

