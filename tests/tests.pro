include( ../common.pri )

QT += core testlib sql
TEMPLATE = app
TARGET = tests
INCLUDEPATH += .
INCLUDEPATH += $${INC_PATH}/
LIBS         += -L$${LIBS_PATH}/ -lorm-qt
DESTDIR       = $${BINS_PATH}

# Input
HEADERS += \
    testactiverecord.h \
    testquery.h
SOURCES += \
    main.cpp \
    testactiverecord.cpp \
    testquery.cpp
