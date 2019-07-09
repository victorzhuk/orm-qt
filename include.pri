QT += sql

DEFINES += ORMQT_LIBRARY

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/query.cpp \
    $$PWD/src/activerecord.cpp

HEADERS += \ 
    $$PWD/src/orm-qt_global.h \
    $$PWD/src/query.h \
    $$PWD/src/activerecord.h