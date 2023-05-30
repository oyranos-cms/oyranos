HEADERS       = qcmsevents.h
SOURCES       = qcmsevents.cpp
RESOURCES     = qcmsevents.qrc
QT           += xml svg widgets x11extras

TARGET = qcmsevents
TARGET_NAME = X\ Color\ Management\ Events
prefix = /opt/local
CONFIG += link_pkgconfig cflags_pkgconfig
PKGCONFIG += x11 xcm xfixes xinerama xrandr xxf86vm oyranos
INCLUDEPATH += . ../.. $$prefix/include ../../include_private ../../../build/src/include_private
DEFINES += HAVE_X11 
LIBS += -lm
QMAKE_LIBDIR += $$prefix/lib ./

