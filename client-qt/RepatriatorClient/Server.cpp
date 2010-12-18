#include "Server.h"

#include <QDir>

#include "IncomingMessageParser.h"
#include "IncomingMessage.h"
#include "OutgoingMessage.h"

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

    success = connect(&m_socket, SIGNAL(connected()), this, SLOT(startReadAndWriteThreads()));
    Q_ASSERT(success);

    success = connect(&m_socket, SIGNAL(disconnected()), this, SLOT(cleanUpAfterDisconnect()));
    Q_ASSERT(success);

    success = connect(m_reader_thread, SIGNAL(messageReceived(IncomingMessage *)), this, SLOT(processIncomingMessage(IncomingMessage *)));
    Q_ASSERT(success);

    success = connect(IncomingMessageParser::instance(), SIGNAL(progress(qint64,qint64)), this, SLOT(emitProgress(qint64,qint64)));
    Q_ASSERT(success);
}

void Server::socketConnect()
{
    changeLoginState(Connecting);
    m_socket.connectToHost(m_connection_info.host, m_connection_info.port);
}

void Server::socketDisconnect()
{
    m_socket.disconnectFromHost();
}

void ReaderThread::run()
{
    forever {
        IncomingMessage * msg = IncomingMessageParser::instance()->readMessage(&m_server->m_socket);
        if (msg == NULL) {
            // end the connecting
            m_server->socketDisconnect();
            break;
        }
        emit messageReceived(msg);
    }
}

void WriterThread::run()
{
    internal = new WriterThreadInternal(this);
    exec();
    delete internal;
}

void WriterThreadInternal::queueMessage(OutgoingMessage *msg)
{
    m_owner->queueMessage(msg);
}

void WriterThread::queueMessage(OutgoingMessage *msg)
{
    QDataStream stream(&m_server->m_socket);
    msg->writeToStream(stream);
    delete msg;
}

void Server::sendMessage(OutgoingMessage *message)
{
    bool success;
    success = QMetaObject::invokeMethod(m_writer_thread->internal, "queueMessage", Q_ARG(OutgoingMessage *, message));
    Q_ASSERT(success);
}

void Server::startReadAndWriteThreads()
{
    // reset state
    m_nextDownloadNumber = 1;
    m_reader_thread = new ReaderThread(this);
    m_writer_thread = new WriterThread(this);

    // kick off threads
    m_reader_thread->start();
    m_writer_thread->start();

    changeLoginState(WaitingForMagicalResponse);
    sendMessage(new MagicalRequestMessage()); // we delete the message after we use it to serialize
}

void Server::cleanUpAfterDisconnect()
{
    // reader thread will end itself when the connection is no longer valid,
    // but we need to tell writer thread to exit.
    m_writer_thread->exit();

    // wait till threads are done
    m_writer_thread->wait();
    delete m_writer_thread;
    m_writer_thread = NULL;

    m_reader_thread->wait();
    delete m_reader_thread;
    m_reader_thread = NULL;

    emit socketDisconnected();
    changeLoginState(Disconnected);
}

void Server::processIncomingMessage(IncomingMessage *msg)
{
    // possibly handle the message (only for the initial setup)
    if (m_login_state == WaitingForMagicalResponse && msg->type == IncomingMessage::MagicalResponse) {
        MagicalResponseMessage * magic_msg = (MagicalResponseMessage *) msg;
        if (magic_msg->is_valid) {
            changeLoginState(WaitingForConnectionResult);
            sendMessage(new ConnectionRequestMessage(c_client_major_version, c_client_minor_version, c_client_build_version, m_connection_info.username, m_password, m_hardware));
            return;
        } else {
            changeLoginState(ServerIsBogus);
            socketDisconnect();
            return;
        }
    } else if (m_login_state == WaitingForConnectionResult && msg->type == IncomingMessage::ConnectionResult) {
        ConnectionResultMessage * connection_result = (ConnectionResultMessage *) msg;
        if (connection_result->connection_status == ConnectionResultMessage::InsufficientPrivileges) {
            changeLoginState(InsufficientPrivileges);
            socketDisconnect();
            return;
        } else if (connection_result->connection_status == ConnectionResultMessage::InvalidLogin) {
            changeLoginState(LoginIsInvalid);
            socketDisconnect();
            return;
        } else if (connection_result->connection_status == ConnectionResultMessage::Success) {
            changeLoginState(Success);
            return;
        } else {
            Q_ASSERT(false);
        }
    }

    // emit it if we didn't handle it
    emit messageReceived(msg);
}

void Server::changeLoginState(LoginStatus state)
{
    m_login_state = state;
    emit loginStatusUpdated(Connecting);
}

void Server::emitProgress(qint64 bytesTransferred, qint64 bytesTotal)
{
    emit progress(bytesTransferred, bytesTotal);
}

QString Server::getNextDownloadFilename()
{
    forever {
        QDir folder(m_connection_info.downloadDirectory);
        QString filename = folder.absoluteFilePath(QString("img_") + QString::number(m_nextDownloadNumber) + QString(".jpg"));
        if (! QFileInfo(filename).exists())
            return filename;
        m_nextDownloadNumber += 1;
    }
}
