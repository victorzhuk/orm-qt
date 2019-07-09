include( ../common.pri )

VERSION = 1.0.1

QT       += sql

QT       -= gui

TARGET = orm-qt
TEMPLATE = lib

DEFINES += ORMQT_LIBRARY

DESTDIR = $${LIBS_PATH}

SOURCES += \
    query.cpp \
    activerecord.cpp

HEADERS += orm-qt_global.h \ 
    query.h \
    activerecord.h
