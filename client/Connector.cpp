#include "Connector.h"

#include "PasswordInputWindow.h"
#include "IncomingMessage.h"

#include <QMessageBox>

Connector::Connector(QSharedPointer<Server> server) :
    m_server(server)
{}

Connector::~Connector()
{}

void Connector::go()
{
    bool success;

    QString password = m_server.data()->connectionSettings()->password;
    if (password.isEmpty()) {
        password = PasswordInputWindow::instance()->showGetPassword(tr("Authentication Required"), tr("&Login"), m_server.data()->connectionSettings()->username);
        if (password.isNull()) {
            emit failure(Cancelled);
            return;
        }
    }

    m_server.data()->setPassword(password);

    success = connect(m_server.data(), SIGNAL(loginStatusUpdated(ServerTypes::LoginStatus)), this, SLOT(updateProgressFromLoginStatus(ServerTypes::LoginStatus)));
    Q_ASSERT(success);

    m_progressDialog = QSharedPointer<QProgressDialog>(new QProgressDialog());
    m_progressDialog.data()->setWindowTitle(tr("Connecting to Server"));
    m_progressDialog.data()->setMinimum(0);
    m_progressDialog.data()->setMaximum(4);
    m_progressDialog.data()->setMinimumDuration(200);
    success = connect(m_progressDialog.data(), SIGNAL(canceled()), this, SLOT(cancel()));
    Q_ASSERT(success);

    m_progressDialog.data()->setValue(0);
    m_server.data()->socketConnect();
}

void Connector::updateProgressFromLoginStatus(ServerTypes::LoginStatus status)
{
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
            cleanup(false);
            emit success();
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
    cleanup();
    emit failure(Cancelled);
}

void Connector::fail(FailureReason reason)
{
    QMessageBox::warning(0, m_progressDialog.data()->labelText(), tr("Unable to connect: ") + m_progressDialog.data()->labelText());
    cleanup();
    emit failure(reason);
}

void Connector::cleanup(bool kill_connection)
{
    disconnect(this, SLOT(updateProgressFromLoginStatus(ServerTypes::LoginStatus)));
    if (kill_connection)
        m_server.data()->socketDisconnect();
}
