#include "Connector.h"

#include "PasswordInputWindow.h"
#include "IncomingMessage.h"

#include <QProgressDialog>

Connector * Connector::create(ConnectionSettings *connection, bool need_hardware)
{
    return new Connector(connection, need_hardware);
}

Connector::Connector(ConnectionSettings * connection, bool need_hardware) :
    m_connection(connection),
    m_need_hardware(need_hardware),
    m_progressDialog(NULL),
    m_server()
{}

void Connector::go()
{
    bool success;

    QString password = m_connection->password;
    if (password.isEmpty()) {
        password = PasswordInputWindow::instance()->showGetPassword(tr("Authentication Required"), tr("&Login"), m_connection->username);
        if (password.isNull()) {
            fail(Cancelled);
            delete this;
            return;
        }
    }

    m_server = QSharedPointer<Server>(new Server(*m_connection, password, m_need_hardware));
    success = connect(m_server.data(), SIGNAL(loginStatusUpdated(int)), this, SLOT(updateProgressFromLoginStatus(int)));
    Q_ASSERT(success);

    m_progressDialog = new QProgressDialog();
    m_progressDialog->setWindowFlags(Qt::Dialog);
    m_progressDialog->setWindowTitle(tr("Connecting to Server"));
    m_progressDialog->setMinimum(0);
    m_progressDialog->setMaximum(4);
    success = connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancel()));
    Q_ASSERT(success);

    m_progressDialog->setValue(0);
    m_server.data()->socketConnect();
}

void Connector::updateProgressFromLoginStatus(int _status)
{
    Server::LoginStatus status = (Server::LoginStatus) _status;
    switch(status) {
        case Server::Disconnected:
            m_progressDialog->setLabelText(tr("Disconnected."));
            m_progressDialog->setValue(4);
            fail(UnableToConnect);
            return;
        case Server::Connecting:
            m_progressDialog->setLabelText(tr("Connecting..."));
            m_progressDialog->setValue(1);
            return;
        case Server::WaitingForMagicalResponse:
            m_progressDialog->setLabelText(tr("Waiting for server validation..."));
            m_progressDialog->setValue(2);
            return;
        case Server::WaitingForConnectionResult:
            m_progressDialog->setLabelText(tr("Waiting for connection result..."));
            m_progressDialog->setValue(3);
            return;
        case Server::ServerIsBogus:
            m_progressDialog->setLabelText(tr("Server is bogus."));
            m_progressDialog->setValue(4);
            fail(ServerIsBogus);
            return;
        case Server::LoginIsInvalid:
            m_progressDialog->setLabelText(tr("Login is invalid."));
            m_progressDialog->setValue(4);
            fail(LoginIsInvalid);
            return;
        case Server::InsufficientPrivileges:
            m_progressDialog->setLabelText(tr("Insufficient privileges."));
            m_progressDialog->setValue(4);
            fail(InsufficientPrivileges);
            return;
        case Server::Success:
            m_progressDialog->setLabelText(tr("Success."));
            m_progressDialog->setValue(4);
            emit success(m_server);
            bye();
            return;
    }
    Q_ASSERT(false);
}

void Connector::cancel()
{
    fail(Cancelled);
}

void Connector::fail(FailureReason reason)
{
    emit failure(reason);
    bye();
}

void Connector::bye()
{
    delete m_progressDialog;
    this->disconnect(); // unhook all signals pointed here
    m_server.data()->socketDisconnect();

    delete this;
}
