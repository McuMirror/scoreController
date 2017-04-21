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
    netServer.cpp

HEADERS  += scorecontroller.h \
    clientlistdialog.h \
    utility.h \
    fileserver.h \
    netServer.h

CONFIG += mobility
MOBILITY = 

