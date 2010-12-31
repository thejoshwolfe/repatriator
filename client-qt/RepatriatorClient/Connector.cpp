#include "Connector.h"

#include "PasswordInputWindow.h"
#include "IncomingMessage.h"

#include <QProgressDialog>
#include <QMessageBox>

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
            emit failure(Cancelled);
            this->disconnect();
            delete this;
            return;
        }
    }

    m_server = QSharedPointer<Server>(new Server(*m_connection, password, m_need_hardware));
    success = connect(m_server.data(), SIGNAL(loginStatusUpdated(ServerTypes::LoginStatus)), this, SLOT(updateProgressFromLoginStatus(ServerTypes::LoginStatus)), Qt::QueuedConnection);
    Q_ASSERT(success);

    m_progressDialog = new QProgressDialog();
    m_progressDialog->setWindowModality(Qt::ApplicationModal);
    m_progressDialog->setWindowFlags(Qt::Dialog);
    m_progressDialog->setWindowTitle(tr("Connecting to Server"));
    m_progressDialog->setMinimum(0);
    m_progressDialog->setMaximum(4);
    m_progressDialog->setMinimumDuration(100);
    success = connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancel()), Qt::DirectConnection);
    Q_ASSERT(success);

    m_progressDialog->setValue(0);
    m_server.data()->socketConnect();
}

void Connector::updateProgressFromLoginStatus(ServerTypes::LoginStatus status)
{
    switch(status) {
        case ServerTypes::Disconnected:
            m_progressDialog->setLabelText(tr("Disconnected."));
            m_progressDialog->setValue(4);
            fail(UnableToConnect);
            return;
        case ServerTypes::Connecting:
            m_progressDialog->setLabelText(tr("Connecting..."));
            m_progressDialog->setValue(1);
            return;
        case ServerTypes::WaitingForMagicalResponse:
            m_progressDialog->setLabelText(tr("Waiting for server validation..."));
            m_progressDialog->setValue(2);
            return;
        case ServerTypes::WaitingForConnectionResult:
            m_progressDialog->setLabelText(tr("Waiting for connection result..."));
            m_progressDialog->setValue(3);
            return;
        case ServerTypes::ServerIsBogus:
            m_progressDialog->setLabelText(tr("Server is bogus."));
            m_progressDialog->setValue(4);
            fail(ServerIsBogus);
            return;
        case ServerTypes::LoginIsInvalid:
            m_progressDialog->setLabelText(tr("Login is invalid."));
            m_progressDialog->setValue(4);
            fail(LoginIsInvalid);
            return;
        case ServerTypes::InsufficientPrivileges:
            m_progressDialog->setLabelText(tr("Insufficient privileges."));
            m_progressDialog->setValue(4);
            fail(InsufficientPrivileges);
            return;
        case ServerTypes::Success:
            m_progressDialog->setLabelText(tr("Success."));
            m_progressDialog->setValue(4);
            emit success(m_server);
            bye();
            return;
        case ServerTypes::SocketError:
            m_progressDialog->setLabelText(tr("Socket error."));
            m_progressDialog->setValue(4);
            fail(UnableToConnect);
            return;
    }
    Q_ASSERT(false);
}

void Connector::cancel()
{
    emit failure(Cancelled);
    bye();
}

void Connector::fail(FailureReason reason)
{
    QMessageBox::warning(0, m_progressDialog->labelText(), tr("Unable to connect: ") + m_progressDialog->labelText());
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
