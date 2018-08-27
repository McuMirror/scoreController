#-------------------------------------------------
#
# Project created by QtCreator 2017-04-21T06:26:26
#
#-------------------------------------------------


#Copyright (C) 2016  Gabriele Salvato

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.


QT += core
QT += gui
QT += network
QT += websockets
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


# to Add a different Build number after a new Build
build_nr.commands = ../scoreController/build_number.sh
build_nr.depends = FORCE
QMAKE_EXTRA_TARGETS += build_nr
PRE_TARGETDEPS += build_nr


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
    gamedirector.cpp \
    generalsetupdialog.cpp \
    directorytab.cpp \
    volleytab.cpp \
    baskettab.cpp \
    handballtab.cpp \
    sportSelector.cpp \
    paneltab.cpp \
    cameratab.cpp

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
    panelconfigurator.h \
    handballcontroller.h \
    gamedirector.h \
    generalsetupdialog.h \
    directorytab.h \
    volleytab.h \
    baskettab.h \
    handballtab.h \
    sportSelector.h \
    paneltab.h \
    cameratab.h \
    paneldirection.h

RESOURCES += scorecontroller.qrc

CONFIG += mobility
MOBILITY = 

FORMS    +=

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
    scoreController_en.ts \
    build_number.sh \
    build_number.sh \
    build_number

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
