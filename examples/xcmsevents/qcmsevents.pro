HEADERS       = qcmsevents.h
SOURCES       = qcmsevents.cpp
RESOURCES     = qcmsevents.qrc
QT           += xml svg

TARGET = qcmsevents
TARGET_NAME = X\ Color\ Management\ Events
prefix = /opt/local
CONFIG += link_pkgconfig cflags_pkgconfig
PKGCONFIG += x11 xcm xmu xfixes xinerama xrandr xxf86vm oyranos
INCLUDEPATH += . ../.. $$prefix/include
DEFINES += HAVE_X11 
LIBS += -lm
QMAKE_LIBDIR += $$prefix/lib ./

