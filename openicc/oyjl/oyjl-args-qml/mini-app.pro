TEMPLATE = app

QT += multimedia qml svg
QTPLUGIN += qsvg

android {
  QT += androidextras
  equals(ANDROID_TARGET_ARCH, armeabi-v7a) {
    INCLUDEPATH+=/run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/include/
    INCLUDEPATH+=/run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/include/oyjl/
    LIBS += -L/run/media/kuwe/KAI-DA/linux/arm-linux-androideabi-4.9/lib/
  }
  equals(ANDROID_TARGET_ARCH, x86) {
    INCLUDEPATH+=/run/media/kuwe/KAI-DA/linux/x86-linux-androideabi-4.9/include/
    LIBS += -L/run/media/kuwe/KAI-DA/linux/x86-linux-androideabi-4.9/lib/
  }
  LIBS += -loyjl-args-qml-static
  LIBS += -loyjl-static
  LIBS += -loyjl-core-static
  LIBS += -lm
  QMAKE_LFLAGS+=-fopenmp
}
unix:!macx:!android {
QT += dbus
INCLUDEPATH+=/home/kuwe/.local/include/
INCLUDEPATH+=/home/kuwe/.local/include/oyjl
INCLUDEPATH+=..
INCLUDEPATH+=/opt/local/include/
LIBS+=-L/home/kuwe/.local/lib64
LIBS+=-L/opt/local/lib64
LIBS+=-fopenmp
#LIBS+=-lOyjlArgsQml
INCLUDEPATH+=/home/kuwe/.local/include/openicc
DEFINES+=OPENICC_LIB
DEFINES+=USE_GETTEXT
LIBS+=-lopenicc-static
LIBS+=-loyjl-static
LIBS+=-loyjl-core-static
LIBS+=-lyaml
LIBS+=-lyajl
LIBS+=-lxml2
LIBS+=-lc
QMAKE_LFLAGS+=-fopenmp
}

SOURCES +=  mini-app.c \
            src/app_data.cpp src/utils.cpp lib.cpp

RESOURCES += app.qrc

TRANSLATIONS = translations/app_de.ts

lupdate_only{
SOURCES += qml/About.qml qml/AppWindow.qml qml/Combo.qml qml/HalfPage.qml qml/LInput.qml qml/LSlider.qml qml/LSwitch.qml qml/main.qml qml/OptionsList.qml qml/SplitView.qml
}

# Additional import path used to resolve QML modules in Qt Creator's code model
# QML_IMPORT_PATH += /run/media/kuwe/KAI-DA/linux/Qt/5.12.2/gcc_64/qml/

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    include/app_data.h \
    include/app_manager.h \
    include/utils.h \
    include/process.h

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/src/org/oyranos/testApp/TestAppActivity.java

DISTFILES += \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
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
