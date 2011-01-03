#include "Server.h"

#include <QDir>

#include "IncomingMessageParser.h"

const int Server::c_client_major_version = 0;
const int Server::c_client_minor_version = 0;
const int Server::c_client_build_version = 0;
const int Server::c_server_major_version = 0;
const int Server::c_server_minor_version = 0;
const int Server::c_server_build_version = 0;

Server::Server(ConnectionSettings connection_info, QString password, bool hardware) :
    m_connection_info(connection_info),
    m_password(password),
    m_hardware(hardware),
    m_reader_thread(NULL),
    m_writer_thread(NULL)
{
    bool success;

    success = connect(&m_socket, SIGNAL(connected()), this, SLOT(startReadAndWriteThreads()), Qt::QueuedConnection);
    Q_ASSERT(success);

    success = connect(&m_socket, SIGNAL(disconnected()), this, SLOT(cleanUpAfterDisconnect()), Qt::QueuedConnection);
    Q_ASSERT(success);

    success = connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)), Qt::DirectConnection);
    Q_ASSERT(success);
}

Server::~Server()
{
    socketDisconnect();
}

void Server::socketConnect()
{
    changeLoginState(ServerTypes::Connecting);
    m_socket.connectToHost(m_connection_info.host, m_connection_info.port);
}

void Server::socketDisconnect()
{
    if (m_socket.isOpen())
        m_socket.disconnectFromHost();
}

void ReaderThread::run()
{
    bool success;

    QScopedPointer<IncomingMessageParser> parser(new IncomingMessageParser(&m_server->m_socket));

    success = connect(parser.data(), SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)));
    Q_ASSERT(success);

    this->exec();
}

void WriterThread::run()
{
    internal = new WriterThreadInternal(this);
    emit ready();
    this->exec();
    delete internal;
}

void WriterThreadInternal::queueMessage(QSharedPointer<OutgoingMessage> msg)
{
    m_owner->queueMessage(msg);
}

void WriterThread::queueMessage(QSharedPointer<OutgoingMessage> msg)
{
    if (dynamic_cast<DummyDisconnectMessage *>(msg.data()) != NULL) {
        m_server->socketDisconnect();
        return;
    }
    if (m_server->m_socket.isOpen()) {
        QDataStream stream(&m_server->m_socket);
        msg.data()->writeToStream(stream);
    }
}

void Server::sendMessage(QSharedPointer<OutgoingMessage> message)
{
    bool success;
    success = QMetaObject::invokeMethod(m_writer_thread->internal, "queueMessage", Qt::QueuedConnection, Q_ARG(QSharedPointer<OutgoingMessage>, message));
    Q_ASSERT(success);
}

void Server::startReadAndWriteThreads()
{
    bool success;

    // reset state
    m_nextDownloadNumber = 1;
    m_reader_thread = new ReaderThread(this);
    m_writer_thread = new WriterThread(this);

    success = connect(m_reader_thread, SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SLOT(processIncomingMessage(QSharedPointer<IncomingMessage>)), Qt::DirectConnection);
    Q_ASSERT(success);
    success = connect(m_writer_thread, SIGNAL(ready()), this, SLOT(handleWriterThreadReady()), Qt::DirectConnection);
    Q_ASSERT(success);

    // kick off threads
    m_reader_thread->start();
    m_writer_thread->start();

}

void Server::handleWriterThreadReady()
{
    changeLoginState(ServerTypes::WaitingForMagicalResponse);
    sendMessage(QSharedPointer<OutgoingMessage>(new MagicalRequestMessage()));
}

void Server::cleanUpAfterDisconnect()
{
    // reader thread will end itself when the connection is no longer valid,
    // but we need to tell writer thread to exit.
    m_writer_thread->exit();
    m_reader_thread->exit();

    // wait till threads are done
    m_writer_thread->wait();
    delete m_writer_thread;
    m_writer_thread = NULL;

    m_reader_thread->wait();
    delete m_reader_thread;
    m_reader_thread = NULL;

    changeLoginState(ServerTypes::Disconnected);
    emit socketDisconnected();
}

void Server::processIncomingMessage(QSharedPointer<IncomingMessage> msg)
{
    // possibly handle the message (only for the initial setup)
    if (msg.isNull()) {
        // end the connection
        qDebug() << "null message, socket disconnect";
        socketDisconnect();
        return;
    } else if (msg.data()->type == IncomingMessage::Ping) {
        // just ignore it
        return;
    } else if (m_login_state == ServerTypes::WaitingForMagicalResponse && msg.data()->type == IncomingMessage::MagicalResponse) {
        MagicalResponseMessage * magic_msg = (MagicalResponseMessage *) msg.data();
        if (magic_msg->is_valid) {
            changeLoginState(ServerTypes::WaitingForConnectionResult);
            sendMessage(QSharedPointer<OutgoingMessage>(new ConnectionRequestMessage(c_client_major_version, c_client_minor_version, c_client_build_version, m_connection_info.username, m_password, m_hardware)));
            return;
        } else {
            changeLoginState(ServerTypes::ServerIsBogus);
            socketDisconnect();
            return;
        }
    } else if (m_login_state == ServerTypes::WaitingForConnectionResult && msg.data()->type == IncomingMessage::ConnectionResult) {
        ConnectionResultMessage * connection_result = (ConnectionResultMessage *) msg.data();
        if (connection_result->connection_status == ConnectionResultMessage::InsufficientPrivileges) {
            changeLoginState(ServerTypes::InsufficientPrivileges);
            socketDisconnect();
            return;
        } else if (connection_result->connection_status == ConnectionResultMessage::InvalidLogin) {
            changeLoginState(ServerTypes::LoginIsInvalid);
            socketDisconnect();
            return;
        } else if (connection_result->connection_status == ConnectionResultMessage::Success) {
            changeLoginState(ServerTypes::Success);
            m_connection_result = msg;
            return;
        } else {
            Q_ASSERT(false);
        }
    }

    // emit it if we didn't handle it
    emit messageReceived(msg);
}

void Server::changeLoginState(ServerTypes::LoginStatus state)
{
    m_login_state = state;
    emit loginStatusUpdated(state);
}

QString Server::getNextDownloadFilename()
{
    forever {
        QDir folder(m_connection_info.download_directory);
        QString filename = folder.absoluteFilePath(QString("img_") + QString::number(m_nextDownloadNumber) + QString(".jpg"));
        if (! QFileInfo(filename).exists())
            return filename;
        m_nextDownloadNumber += 1;
    }
}

void Server::handleSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error: " << error;
    changeLoginState(ServerTypes::SocketError);
}

void Server::finishWritingAndDisconnect()
{
    // put a dummy message on the queue
    this->sendMessage(QSharedPointer<OutgoingMessage>(new DummyDisconnectMessage()));
}

ServerTypes::LoginStatus Server::loginStatus()
{
    return m_login_state;
}
