#-------------------------------------------------
#
# Project created by QtCreator 2017-04-21T06:26:26
#
#-------------------------------------------------

QT += core
QT += gui
QT += network
QT += websockets
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scoreController
TEMPLATE = app


SOURCES += main.cpp\
    scorecontroller.cpp \
    clientlistdialog.cpp \
    utility.cpp \
    fileserver.cpp \
    netServer.cpp \
    choosediscilpline.cpp \
    basketcontroller.cpp \
    volleycontroller.cpp \
    button.cpp \
    edit.cpp \
    radioButton.cpp \
    panelconfigurator.cpp

HEADERS  += scorecontroller.h \
    clientlistdialog.h \
    utility.h \
    fileserver.h \
    netServer.h \
    choosediscilpline.h \
    basketcontroller.h \
    volleycontroller.h \
    button.h \
    edit.h \
    radioButton.h \
    panelorientation.h \
    panelconfigurator.h

RESOURCES += scorecontroller.qrc

CONFIG += mobility
MOBILITY = 

FORMS    += choosediscilpline.ui \
    panelconfigurator.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
