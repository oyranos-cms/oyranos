TEMPLATE = app

QT += multimedia qml svg
QTPLUGIN += qsvg
CONFIG += LOCAL_ARGS # DYNAMIC_LIB_ARGS # STATIC_LIB_ARGS # LOCAL_ARGS

android {
  QT += androidextras
  equals(ANDROID_TARGET_ARCH, armeabi-v7a) {
    INCLUDEPATH+=/run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/include/
    INCLUDEPATH+=/run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/include/oyjl/
    LIBS += -L/run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/lib/
    #ANDROID_EXTRA_LIBS += /run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/lib/libOyjlArgsQml.so \
    #                      /run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/lib/libOyjlCore.so
  }
  equals(ANDROID_TARGET_ARCH, x86) {
    INCLUDEPATH+=/run/media/kuwe/KAI-DA/linux/x86-linux-androideabi-4.9/include/
    LIBS += -L/run/media/kuwe/KAI-DA/linux/x86-linux-androideabi-4.9/lib/
  }
  QMAKE_LFLAGS+=-fopenmp
  LIBS += -lm
  LIBS += -ldl
}
unix:!macx:!android {
QT += dbus
INCLUDEPATH += /home/kuwe/.local/include/
INCLUDEPATH += /home/kuwe/.local/include/oyjl
INCLUDEPATH += /opt/local/include/
INCLUDEPATH += /home/kuwe/.local/include/openicc
INCLUDEPATH += ..
INCLUDEPATH += ../../../build-oyjl
INCLUDEPATH += ../../src/include # openicc src
INCLUDEPATH += ../../../build-openicc/src/include
LIBS += -L/home/kuwe/.local/lib64
LIBS += -L/opt/local/lib64
}

DYNAMIC_LIB_ARGS { # dlopen libOyjlArgsQml
  LIBS += -lOyjlArgsQml
  LIBS += -lOyjlCore
  LIBS += -ldl
} else:STATIC_LIB_ARGS { # link statically liboyjl-args-qml-static
  DEFINES += COMPILE_STATIC
  LIBS += -loyjl-args-qml-static
  LIBS += -lopenicc-static
  LIBS += -loyjl-static
  LIBS += -loyjl-core-static
  LIBS += -lyaml
  LIBS += -lyajl
  LIBS += -lxml2
  LIBS += -lc
} else:LOCAL_ARGS { # compile in all oyjlArgsQml symbols for QML debugging
  DEFINES += COMPILE_STATIC
  SOURCES += src/app_data.cpp src/utils.cpp lib.cpp
  LIBS += -loyjl-core-static
  DISTFILES += \
    qml/About.qml \
    qml/AppWindow.qml \
    qml/HalfPage.qml \
    qml/main.qml \
    qml/process.js \
    translations/app_de.ts \
    qml/Combo.qml \
    qml/LInput.qml \
    qml/LSlider.qml \
    qml/LSwitch.qml \
    qml/OptionsList.qml \
    qml/SplitView.qml
  RESOURCES += app.qrc
  unix:!macx:!android {
    LIBS += -L../../../build-oyjl
    LIBS += -L../../../build-openicc/src/libopenicc
    DEFINES += USE_GETTEXT
    LIBS += -lopenicc-static
    LIBS += -loyjl-static
    LIBS += -lxml2
    LIBS += -lyaml
    LIBS += -lyajl
  }
  LIBS+=-lc
  QMAKE_LFLAGS+=-fopenmp

  HEADERS += \
    include/app_data.h \
    include/app_manager.h \
    include/utils.h \
    include/process.h

  TRANSLATIONS = translations/app_de.ts

  lupdate_only{
    SOURCES += qml/About.qml qml/AppWindow.qml qml/Combo.qml qml/HalfPage.qml qml/LInput.qml qml/LSlider.qml qml/LSwitch.qml qml/main.qml qml/OptionsList.qml qml/SplitView.qml
  }
}

SOURCES +=   mini-app.c


# Additional import path used to resolve QML modules in Qt Creator's code model
# QML_IMPORT_PATH += /run/media/kuwe/KAI-DA/linux/Qt/5.12.2/gcc_64/qml/

# Default rules for deployment.
include(deployment.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/src/org/oyranos/testApp/TestAppActivity.java

DISTFILES += \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png
