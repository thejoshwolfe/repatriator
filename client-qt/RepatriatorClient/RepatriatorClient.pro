#-------------------------------------------------
#
# Project created by QtCreator 2010-12-17T16:29:53
#
#-------------------------------------------------

QT += core gui network

TARGET = RepatriatorClient
TEMPLATE = app


SOURCES += \
    main.cpp \
    MainWindow.cpp \
    IncomingMessage.cpp \
    OutgoingMessage.cpp \
    Server.cpp \
    IncomingMessageParser.cpp \
    ConnectionWindow.cpp \
    AdminWindow.cpp \
    EditConnectionWindow.cpp \
    EditUserAccountWindow.cpp \
    PasswordInputWindow.cpp \
    Settings.cpp \
    ConnectionSettings.cpp \
    Connector.cpp

HEADERS += \
    MainWindow.h \
    ConnectionSettings.h \
    IncomingMessage.h \
    OutgoingMessage.h \
    Server.h \
    IncomingMessageParser.h \
    ConnectionWindow.h \
    AdminWindow.h \
    EditConnectionWindow.h \
    EditUserAccountWindow.h \
    PasswordInputWindow.h \
    Settings.h \
    Connector.h

FORMS += \
    MainWindow.ui \
    ConnectionWindow.ui \
    AdminWindow.ui \
    EditConnectionWindow.ui \
    EditUserAccountWindow.ui \
    PasswordInputWindow.ui

RESOURCES += \
    repatriator.qrc
