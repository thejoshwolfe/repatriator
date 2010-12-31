#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "ConnectionSettings.h"
#include "IncomingMessage.h"
#include "OutgoingMessage.h"
#include "ServerTypes.h"

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QSharedPointer>

class ReaderThread;
class WriterThread;

// MessageHandler has a QTcpSocket to talk to the server.
// use it to send messages and connect to it to hear it emit incoming messages.
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(ConnectionSettings connection_info, QString password, bool hardware);
    ~Server();

signals:
    // use this signal to listen for incoming messages
    // YOU are responsible for deleting them when done.
    // that means if you don't listen to this signal, you have a giant
    // memory leak.
    void messageReceived(QSharedPointer<IncomingMessage> message);

    void loginStatusUpdated(ServerTypes::LoginStatus status);
    void socketDisconnected();

    // gives you progress of incoming messages.
    // don't try to access the data of message because it will be garbage. do that in
    // messageReceived. use it only to tell messages apart.
    void progress(qint64 bytesTransferred, qint64 bytesTotal, IncomingMessage * message);

public slots:
    void sendMessage(QSharedPointer<OutgoingMessage> message);

    // use this to actually connect to the server
    void socketConnect();
    void socketDisconnect();

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

    ServerTypes::LoginStatus m_login_state;

    int m_nextDownloadNumber;

private:
    void changeLoginState(ServerTypes::LoginStatus state);
    QString getNextDownloadFilename();

private slots:
    void startReadAndWriteThreads();
    void cleanUpAfterDisconnect();
    void processIncomingMessage(QSharedPointer<IncomingMessage>);
    void handleSocketError(QAbstractSocket::SocketError);
    void handleWriterThreadReady();

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
    void messageReceived(QSharedPointer<IncomingMessage> message);
private:
    Server * m_server;
};


class WriterThreadInternal;
class WriterThread : public QThread
{
    Q_OBJECT
public:
    WriterThread(Server * server) : m_server(server) {}
    void run();

    // internal class so that this thread "owns" the object so that
    // queued connections operate correctly.
    WriterThreadInternal * internal;

signals:
    // emitted after the thread is running and done initializing
    void ready();

private:
    Server * m_server;

    void queueMessage(QSharedPointer<OutgoingMessage> msg);

    friend class WriterThreadInternal;
};

class WriterThreadInternal : public QObject
{
    Q_OBJECT
public:
    WriterThreadInternal(WriterThread * owner) : m_owner(owner) {}
public slots:
    void queueMessage(QSharedPointer<OutgoingMessage> msg);
private:
    WriterThread * m_owner;
};

#endif
