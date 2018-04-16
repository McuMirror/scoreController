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
QT += testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scoreController
TEMPLATE = app

TRANSLATIONS = scoreController_en.ts


SOURCES += main.cpp\
    scorecontroller.cpp \
    clientlistdialog.cpp \
    utility.cpp \
    fileserver.cpp \
    netServer.cpp \
    basketcontroller.cpp \
    volleycontroller.cpp \
    button.cpp \
    edit.cpp \
    radioButton.cpp \
    panelconfigurator.cpp \
    handballcontroller.cpp \
    choosediscipline.cpp \
    gamedirector.cpp


HEADERS  += scorecontroller.h \
    clientlistdialog.h \
    utility.h \
    fileserver.h \
    netServer.h \
    basketcontroller.h \
    volleycontroller.h \
    button.h \
    edit.h \
    radioButton.h \
    panelorientation.h \
    panelconfigurator.h \
    handballcontroller.h \
    choosediscipline.h \
    gamedirector.h

RESOURCES += scorecontroller.qrc

CONFIG += mobility
MOBILITY = 

FORMS    += \
    panelconfigurator.ui \
    choosediscipline.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    scoreController_en.ts

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
