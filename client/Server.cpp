#include "Server.h"

#include "IncomingMessageParser.h"

#include <QDir>
#include <QCoreApplication>

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
    m_socket_thread(NULL),
    m_socket(NULL),
    m_parser(NULL),
    m_next_ping(0),
    m_ping_timer(NULL)
{
    // we run in m_socket_thread
    m_socket_thread = new QThread(this);
    m_socket_thread->start();
    this->moveToThread(m_socket_thread);

    bool success;
    success = QMetaObject::invokeMethod(this, "initialize", Qt::QueuedConnection);
    Q_ASSERT(success);
}

Server::~Server()
{
    delete m_parser;
    delete m_ping_timer;
}

void Server::initialize()
{
    Q_ASSERT(QThread::currentThread() == m_socket_thread);

    m_socket = new QTcpSocket(this);

    bool success;

    success = connect(m_socket, SIGNAL(connected()), this, SLOT(handleConnected()));
    Q_ASSERT(success);

    success = connect(m_socket, SIGNAL(disconnected()), this, SLOT(cleanUpAfterDisconnect()));
    Q_ASSERT(success);

    success = connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    Q_ASSERT(success);
}

void Server::socketConnect()
{
    if (QThread::currentThread() != m_socket_thread) {
        bool success = QMetaObject::invokeMethod(this, "socketConnect", Qt::QueuedConnection);
        Q_ASSERT(success);
        return;
    }
    Q_ASSERT(m_socket);

    changeLoginState(ServerTypes::Connecting);
    m_socket->connectToHost(m_connection_info.host, m_connection_info.port);
}

void Server::socketDisconnect()
{
    if (QThread::currentThread() != m_socket_thread) {
        bool success = QMetaObject::invokeMethod(this, "socketDisconnect", Qt::QueuedConnection);
        Q_ASSERT(success);
        return;
    }

    if (m_socket->isOpen())
        m_socket->disconnectFromHost();
}

void Server::sendMessage(QSharedPointer<OutgoingMessage> msg)
{
    if (QThread::currentThread() != m_socket_thread) {
        bool success = QMetaObject::invokeMethod(this, "sendMessage", Qt::QueuedConnection, QGenericReturnArgument(), Q_ARG(QSharedPointer<OutgoingMessage>, msg));
        Q_ASSERT(success);
        return;
    }

    if (dynamic_cast<DummyDisconnectMessage *>(msg.data()) != NULL) {
        socketDisconnect();
        return;
    }
    if (m_socket->isOpen()) {
        QDataStream stream(m_socket);
        msg.data()->writeToStream(stream);
    }
}

void Server::handleConnected()
{
    delete m_parser;
    m_parser = new IncomingMessageParser(m_socket);

    delete m_ping_timer;
    m_ping_timer = new QTimer();

    m_next_ping = 0;

    bool success;

    success = connect(m_parser, SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SLOT(processIncomingMessage(QSharedPointer<IncomingMessage>)));
    Q_ASSERT(success);

    success = connect(m_parser, SIGNAL(progress(qint64,qint64,IncomingMessage*)), this, SIGNAL(progress(qint64,qint64,IncomingMessage*)));
    Q_ASSERT(success);

    success = connect(m_ping_timer, SIGNAL(timeout()), this, SLOT(sendPingMessage()));
    Q_ASSERT(success);

    m_ping_timer->start(1000);

    changeLoginState(ServerTypes::WaitingForMagicalResponse);
    sendMessage(QSharedPointer<OutgoingMessage>(new MagicalRequestMessage()));
}

void Server::cleanUpAfterDisconnect()
{
    qDebug() << "Cleaning up, disconnected";
    m_socket_thread->exit();
    this->moveToThread(QCoreApplication::instance()->thread());
    bool success;
    success = QMetaObject::invokeMethod(this, "terminate", Qt::QueuedConnection);
    Q_ASSERT(success);
}

void Server::terminate()
{
    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
    m_socket_thread->exit();
    m_socket_thread->wait();

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
    } else if (msg.data()->type == IncomingMessage::Pong) {
        PongMessage * pong_msg = (PongMessage *) msg.data();
        if (m_recent_ping == pong_msg->ping_id)
            emit pingComputed(m_ping_time.elapsed());
        return;
    } else if (m_login_state == ServerTypes::WaitingForMagicalResponse && msg.data()->type == IncomingMessage::MagicalResponse) {
        MagicalResponseMessage * magic_msg = (MagicalResponseMessage *) msg.data();
        if (magic_msg->is_valid) {
            changeLoginState(ServerTypes::WaitingForConnectionResult);
            sendMessage(QSharedPointer<OutgoingMessage>(new ConnectionRequestMessage(0, m_connection_info.username, m_password, m_hardware)));
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

void Server::sendPingMessage()
{
    m_recent_ping = m_next_ping++;
    m_ping_time.start();
    sendMessage(QSharedPointer<OutgoingMessage>(new PingMessage(m_recent_ping)));
}

void Server::setPassword(QString password)
{
    m_password = password;
}

void Server::setNeedHardware(bool need_hardware)
{
    m_hardware = need_hardware;
}
