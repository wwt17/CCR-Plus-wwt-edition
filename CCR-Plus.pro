#-------------------------------------------------
#
# Project created by QtCreator 2014-12-18T19:21:25
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CCR-Plus
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    problem.cpp \
    judgethread.cpp \
    boardtable.cpp \
    detailtable.cpp \
    global.cpp \
    configdialog.cpp \
    itemdelegate.cpp \
    createfiledialog.cpp

HEADERS  += mainwindow.h \
    player.h \
    problem.h \
    judgethread.h \
    global.h \
    boardtable.h \
    detailtable.h \
    header.h \
    configdialog.h \
    itemdelegate.h \
    createfiledialog.h

FORMS    += mainwindow.ui \
    configdialog.ui \
    createfiledialog.ui

QMAKE_CXXFLAGS += -std=c++0x

RESOURCES += \
    image.qrc

RC_FILE += res.rc

win32: LIBS += -lpsapi
