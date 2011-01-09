#include "AdminWindow.h"
#include "ui_AdminWindow.h"

#include "OutgoingMessage.h"
#include "PasswordInputWindow.h"
#include "EditUserAccountWindow.h"

#include <QDialogButtonBox>
#include <QMessageBox>

AdminWindow * AdminWindow::s_instance = NULL;

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow),
    m_got_users(false)
{
    ui->setupUi(this);

    bool success;
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);

    enableCorrectControls();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

AdminWindow * AdminWindow::instance()
{
    if (! s_instance)
        s_instance = new AdminWindow();
    return s_instance;
}

void AdminWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AdminWindow::showAdmin(ConnectionSettings *connection)
{
    // clear ui
    ui->usersList->clear();
    ui->usersList->addItem(tr("Updating..."));
    ui->usersList->setFocus(Qt::OtherFocusReason);

    // establish connection
    m_server = QSharedPointer<Server>(new Server(*connection));
    m_connector = QSharedPointer<Connector>(new Connector(m_server));

    bool success;
    success = connect(m_connector.data(), SIGNAL(failure(Connector::FailureReason)), this, SLOT(connectionFailure(Connector::FailureReason)), Qt::QueuedConnection);
    Q_ASSERT(success);
    success = connect(m_connector.data(), SIGNAL(success()), this, SLOT(connected()));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SLOT(processMessage(QSharedPointer<IncomingMessage>)));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(socketDisconnected()), this, SLOT(connectionEnded()));
    Q_ASSERT(success);

    m_connector.data()->go();

    // show modal
    this->exec();
}

void AdminWindow::updateUserList(QList<ServerTypes::UserInfo> users)
{
    ui->usersList->clear();
    m_users.clear();
    foreach (ServerTypes::UserInfo user, users) {
        ui->usersList->addItem(user.username);
        m_users.insert(user.username, QSharedPointer<DetailedUserInfo>(new DetailedUserInfo(user)));
    }

    m_got_users = true;
    enableCorrectControls();
}

void AdminWindow::handleRejected()
{
    cleanup();
}

void AdminWindow::connected()
{
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new ListUserRequestMessage()));
}

void AdminWindow::connectionFailure(Connector::FailureReason reason)
{
    Q_UNUSED(reason);
    reject();
}

void AdminWindow::connectionEnded()
{
    this->close();
    cleanup();
}

void AdminWindow::cleanup()
{
    m_server.clear();
}

void AdminWindow::handleAccepted()
{
    // apply changes
    foreach (QSharedPointer<DetailedUserInfo> user, m_users)
    {
        switch (user.data()->changed_status)
        {
            case DetailedUserInfo::New:
                m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new AddUserMessage(user.data()->username, user.data()->password, user.data()->permissions)));
                break;
            case DetailedUserInfo::Updated:
                m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new UpdateUserMessage(user.data()->username, user.data()->password, user.data()->permissions)));
                break;
            case DetailedUserInfo::Deleted:
                m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new DeleteUserMessage(user.data()->username)));
                break;
            case DetailedUserInfo::Unchanged:
                // do nothing
                break;
        }
    }

    m_server.data()->finishWritingAndDisconnect();
}

void AdminWindow::processMessage(QSharedPointer<IncomingMessage> msg)
{
    switch (msg.data()->type) {
        case IncomingMessage::ListUserResult:
        {
            ListUserResultMessage * user_list_msg = (ListUserResultMessage *) msg.data();
            updateUserList(user_list_msg->users);
            break;
        }
        case IncomingMessage::ErrorMessage:
        {
            ErrorMessage * err_msg = (ErrorMessage *) msg.data();
            QMessageBox::warning(this, tr("Message from Server"), err_msg->message, QMessageBox::Ok);
            qDebug() << "Error message: " << err_msg->message;
            break;
        }
        default:
            qDebug() << "wtf got a message " << msg.data()->type;
            Q_ASSERT(false);
    }
}

void AdminWindow::on_changePasswordButton_clicked()
{
    Q_ASSERT(ui->usersList->selectedItems().count() == 1);

    QString username = ui->usersList->selectedItems().at(0)->text();
    Q_ASSERT(! username.isEmpty());

    QSharedPointer<DetailedUserInfo> user = m_users.value(username);

    QString new_password = PasswordInputWindow::instance()->showGetPassword(tr("Change Password"), tr("&OK"), user.data()->username);

    if (new_password.isEmpty())
        return;

    user.data()->password = new_password;
    user.data()->changed_status = DetailedUserInfo::Updated;
}

void AdminWindow::on_usersList_itemSelectionChanged()
{
    if (! ui->usersList->isEnabled())
        return;

    enableCorrectControls();

    if (ui->usersList->selectedItems().count() == 1) {
        QString username = ui->usersList->selectedItems().at(0)->text();
        QSharedPointer<DetailedUserInfo> user = m_users.value(username);
        ui->adminPrivilegesCheckBox->setChecked(user.data()->permissions.contains(ServerTypes::ManageUsers));
    } else {
        // no selection
        ui->adminPrivilegesCheckBox->setChecked(false);
    }
}

void AdminWindow::enableCorrectControls()
{
    ui->usersList->setEnabled(m_got_users);
    ui->newButton->setEnabled(m_got_users);
    // the ok button from the button box
    foreach (QAbstractButton * button, ui->buttonBox->buttons()) {
        if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
            button->setEnabled(m_got_users);
    }

    bool user_is_selected = ui->usersList->selectedItems().count() == 1;

    ui->changePasswordButton->setEnabled(m_got_users && user_is_selected);
    ui->deleteButton->setEnabled(m_got_users && user_is_selected);
    ui->adminPrivilegesCheckBox->setEnabled(m_got_users && user_is_selected);

}

void AdminWindow::on_deleteButton_clicked()
{
    Q_ASSERT(ui->usersList->selectedItems().count() == 1);

    QString username = ui->usersList->selectedItems().at(0)->text();
    Q_ASSERT(! username.isEmpty());

    QSharedPointer<DetailedUserInfo> user = m_users.value(username);
    if (user.data()->changed_status == DetailedUserInfo::New) {
        // i changed my mind about creating this user
        m_users.remove(username);
    } else {
        // we need to be sure to delete this guy
        user.data()->changed_status = DetailedUserInfo::Deleted;
    }
    delete ui->usersList->takeItem(ui->usersList->row(ui->usersList->selectedItems().at(0)));
}

void AdminWindow::on_newButton_clicked()
{
    QSharedPointer<EditUserAccountWindow::UserAccount> new_account = EditUserAccountWindow::instance()->showGetNewUser();

    if (new_account.isNull())
        return;

    QSharedPointer<DetailedUserInfo> user = QSharedPointer<DetailedUserInfo>(new DetailedUserInfo());
    user.data()->username = new_account.data()->username;
    user.data()->password = new_account.data()->password;

    user.data()->permissions.insert(ServerTypes::OperateHardware);
    if (new_account.data()->is_admin)
        user.data()->permissions.insert(ServerTypes::ManageUsers);

    if (m_users.contains(user.data()->username)) {
        // previously deleted. turn this into an update
        user.data()->changed_status = DetailedUserInfo::Updated;
        m_users.insert(user.data()->username, user);
    } else {
        // actually new
        user.data()->changed_status = DetailedUserInfo::New;
        m_users.insert(user.data()->username, user);
    }

    ui->usersList->addItem(user.data()->username);
    ui->usersList->setCurrentRow(ui->usersList->count() - 1);
}
