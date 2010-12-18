#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "ConnectionSettings.h"

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QImage>

class IncomingMessage;
class OutgoingMessage;
class ReaderThread;
class WriterThread;

// MessageHandler has a QTcpSocket to talk to the server.
// use it to send messages and connect to it to hear it emit incoming messages.
class Server : public QObject
{
    Q_OBJECT
public:
    enum LoginStatus
    {
        Disconnected,
        Connecting,
        WaitingForMagicalResponse,
        WaitingForConnectionResult,
        ServerIsBogus,
        LoginIsInvalid,
        InsufficientPrivileges,
        Success,
    };

    enum Permission
    {
        OperateHardware = 0,
        ManageUsers = 1,
    };

    class UserInfo
    {
    public:
        QString username;
        QSet<Permission> permissions;
    };

    class DirectoryItem
    {
    public:
        QString filename;
        QImage thumbnail;
    };

public slots:
    void sendMessage(OutgoingMessage * message);

    void socketConnect();
    void socketDisconnect();

signals:
    void messageReceived(IncomingMessage * message);
    void loginStatusUpdated(LoginStatus status);
    void socketDisconnected();
    void progress(qint64 bytesTransferred, qint64 bytesTotal);

public:
    explicit Server(ConnectionSettings connection_info, QString password, bool hardware);

private:
    static const int c_client_major_version;
    static const int c_client_minor_version;
    static const int c_client_build_version;
    static const int c_server_major_version;
    static const int c_server_minor_version;
    static const int c_server_build_version;


    ConnectionSettings m_connection_info;
    QString m_password;
    bool m_hardware;

    ReaderThread * m_reader_thread;
    WriterThread * m_writer_thread;

    QTcpSocket m_socket;

    LoginStatus m_login_state;

    int m_nextDownloadNumber;

private:
    void changeLoginState(LoginStatus state);
    QString getNextDownloadFilename();

private slots:
    void startReadAndWriteThreads();
    void cleanUpAfterDisconnect();
    void processIncomingMessage(IncomingMessage * msg);
    void emitProgress(qint64 bytesTransferred, qint64 bytesTotal);

    friend class ReaderThread;
    friend class WriterThread;
};

class ReaderThread : public QThread
{
    Q_OBJECT
public:
    ReaderThread(Server * server) : m_server(server) {}
    void run();
signals:
    void messageReceived(IncomingMessage * message);
private:
    Server * m_server;
};


class WriterThreadInternal;
class WriterThread : public QThread
{
public:
    WriterThread(Server * server) : m_server(server) {}
    void run();

    // internal class so that this thread "owns" the object so that
    // queued connections operate correctly.
    WriterThreadInternal * internal;

private:
    Server * m_server;

    void queueMessage(OutgoingMessage * msg);

    friend class WriterThreadInternal;
};

class WriterThreadInternal : public QObject
{
    Q_OBJECT
public:
    WriterThreadInternal(WriterThread * owner) : m_owner(owner) {}
public slots:
    void queueMessage(OutgoingMessage * msg);
private:
    WriterThread * m_owner;
};

#endif
