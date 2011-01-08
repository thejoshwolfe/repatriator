# -------------------------------------------------
# Project created by QtCreator 2010-12-17T16:29:53
# -------------------------------------------------
QT += core \
    gui \
    network
TARGET = RepatriatorClient
TEMPLATE = app
CONFIG -= exceptions
SOURCES += main.cpp \
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
    Connector.cpp \
    ShadowMinimap.cpp \
    ShadowSlider.cpp \
    Rotate3DWidget.cpp \
    ImageDisplayWidget.cpp \
    EditBookmarkDialog.cpp
HEADERS += MainWindow.h \
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
    Connector.h \
    ServerTypes.h \
    MetaTypes.h \
    ShadowMinimap.h \
    ShadowSlider.h \
    Rotate3DWidget.h \
    ImageDisplayWidget.h \
    EditBookmarkDialog.h
FORMS += MainWindow.ui \
    ConnectionWindow.ui \
    AdminWindow.ui \
    EditConnectionWindow.ui \
    EditUserAccountWindow.ui \
    PasswordInputWindow.ui \
    EditBookmarkDialog.ui
RESOURCES += repatriator.qrc
RC_FILE = repatriator.rc
