#include "Connector.h"

#include "PasswordInputWindow.h"
#include "IncomingMessage.h"

#include <QMessageBox>

Connector::Connector(ConnectionSettings * connection, bool need_hardware) :
    m_connection(connection),
    m_need_hardware(need_hardware),
    m_progressDialog(),
    m_server(),
    m_done(false)
{}

Connector::~Connector()
{}

void Connector::go()
{
    bool success;

    QString password = m_connection->password;
    if (password.isEmpty()) {
        password = PasswordInputWindow::instance()->showGetPassword(tr("Authentication Required"), tr("&Login"), m_connection->username);
        if (password.isNull()) {
            emit failure(Cancelled);
            this->disconnect();
            return;
        }
    }

    m_server = QSharedPointer<Server>(new Server(*m_connection, password, m_need_hardware));
    success = connect(m_server.data(), SIGNAL(loginStatusUpdated(ServerTypes::LoginStatus)), this, SLOT(updateProgressFromLoginStatus(ServerTypes::LoginStatus)), Qt::QueuedConnection);
    Q_ASSERT(success);

    m_progressDialog = QSharedPointer<QProgressDialog>(new QProgressDialog());
    m_progressDialog.data()->setWindowModality(Qt::ApplicationModal);
    m_progressDialog.data()->setWindowFlags(Qt::Dialog);
    m_progressDialog.data()->setWindowTitle(tr("Connecting to Server"));
    m_progressDialog.data()->setMinimum(0);
    m_progressDialog.data()->setMaximum(4);
    m_progressDialog.data()->setMinimumDuration(100);
    success = connect(m_progressDialog.data(), SIGNAL(canceled()), this, SLOT(cancel()));
    Q_ASSERT(success);

    m_progressDialog.data()->setValue(0);
    m_server.data()->socketConnect();
}

void Connector::updateProgressFromLoginStatus(ServerTypes::LoginStatus status)
{
    if (m_done)
        return;

    switch(status) {
        case ServerTypes::Connecting:
            m_progressDialog.data()->setLabelText(tr("Connecting..."));
            m_progressDialog.data()->setValue(1);
            return;
        case ServerTypes::WaitingForMagicalResponse:
            m_progressDialog.data()->setLabelText(tr("Waiting for server validation..."));
            m_progressDialog.data()->setValue(2);
            return;
        case ServerTypes::WaitingForConnectionResult:
            m_progressDialog.data()->setLabelText(tr("Waiting for connection result..."));
            m_progressDialog.data()->setValue(3);
            return;
        case ServerTypes::ServerIsBogus:
            m_progressDialog.data()->setLabelText(tr("Server is bogus."));
            m_progressDialog.data()->setValue(4);
            fail(ServerIsBogus);
            return;
        case ServerTypes::LoginIsInvalid:
            m_progressDialog.data()->setLabelText(tr("Login is invalid."));
            m_progressDialog.data()->setValue(4);
            fail(LoginIsInvalid);
            return;
        case ServerTypes::InsufficientPrivileges:
            m_progressDialog.data()->setLabelText(tr("Insufficient privileges."));
            m_progressDialog.data()->setValue(4);
            fail(InsufficientPrivileges);
            return;
        case ServerTypes::Success:
            m_progressDialog.data()->setLabelText(tr("Success."));
            m_progressDialog.data()->setValue(4);
            emit success(m_server);
            cleanup(false);
            return;
        case ServerTypes::SocketError:
            m_progressDialog.data()->setLabelText(tr("Socket error."));
            m_progressDialog.data()->setValue(4);
            fail(UnableToConnect);
            return;
        case ServerTypes::Disconnected:
            return;
    }
    Q_ASSERT(false);
}

void Connector::cancel()
{
    emit failure(Cancelled);
    cleanup();
}

void Connector::fail(FailureReason reason)
{
    QMessageBox::warning(0, m_progressDialog.data()->labelText(), tr("Unable to connect: ") + m_progressDialog.data()->labelText());
    emit failure(reason);
    cleanup();
}

void Connector::cleanup(bool kill_connection)
{
    m_done = true;
    disconnect(this);
    if (kill_connection)
        m_server.data()->socketDisconnect();
}
