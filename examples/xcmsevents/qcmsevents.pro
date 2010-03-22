HEADERS       = qcmsevents.h
SOURCES       = qcmsevents.cpp \
                xcmsevents_common.c
RESOURCES     = qcmsevents.qrc
QT           += xml svg

TARGET = qcmsevents
TARGET_NAME = X\ Color\ Management\ Events
prefix = /opt/local
CONFIG += link_pkgconfig cflags_pkgconfig
PKGCONFIG += x11 xmu xfixes xinerama xrandr xxf86vm oyranos
INCLUDEPATH += . ../.. $$prefix/include
DEFINES += HAVE_X11 
LIBS += -lXcolor -loyranosedid 
QMAKE_LIBDIR += $$prefix/lib ./

