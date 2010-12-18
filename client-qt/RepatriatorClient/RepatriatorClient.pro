#-------------------------------------------------
#
# Project created by QtCreator 2010-12-17T16:29:53
#
#-------------------------------------------------

QT       += core gui network

TARGET = RepatriatorClient
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    IncomingMessage.cpp \
    OutgoingMessage.cpp \
    Server.cpp \
    IncomingMessageParser.cpp

HEADERS  += MainWindow.h \
    ConnectionSettings.h \
    IncomingMessage.h \
    OutgoingMessage.h \
    Server.h \
    IncomingMessageParser.h

FORMS    += MainWindow.ui
