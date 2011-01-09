#include "EditUserAccountWindow.h"
#include "ui_EditUserAccountWindow.h"

EditUserAccountWindow * EditUserAccountWindow::s_instance = NULL;

EditUserAccountWindow::EditUserAccountWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditUserAccountWindow),
    m_account()
{
    ui->setupUi(this);

    bool success;
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
}

EditUserAccountWindow::~EditUserAccountWindow()
{
    delete ui;
}

void EditUserAccountWindow::changeEvent(QEvent *e)
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

EditUserAccountWindow * EditUserAccountWindow::instance()
{
    if (! s_instance)
        s_instance = new EditUserAccountWindow();
    return s_instance;
}

QSharedPointer<EditUserAccountWindow::UserAccount> EditUserAccountWindow::showGetNewUser()
{
    m_account = QSharedPointer<UserAccount>(new UserAccount());

    // clear ui
    ui->usernameLineEdit->setText("");
    ui->passwordLineEdit->setText("");
    ui->adminCheckBox->setChecked(false);
    ui->usernameLineEdit->setFocus(Qt::OtherFocusReason);

    this->setWindowTitle(tr("New User Account"));
    this->exec();

    return m_account;
}

void EditUserAccountWindow::showEditUser(QSharedPointer<UserAccount> account)
{
    m_account = account;

    ui->usernameLineEdit->setText(m_account.data()->username);
    ui->passwordLineEdit->setText(m_account.data()->password);
    ui->adminCheckBox->setChecked(m_account.data()->is_admin);
    ui->usernameLineEdit->setFocus(Qt::OtherFocusReason);

    this->setWindowTitle(tr("Edit User Account"));
    this->exec();
}

void EditUserAccountWindow::handleAccepted()
{
    m_account.data()->username = ui->usernameLineEdit->text();
    m_account.data()->password = ui->passwordLineEdit->text();
    m_account.data()->is_admin = ui->adminCheckBox->isChecked();
}

void EditUserAccountWindow::handleRejected()
{
    m_account.clear();
    Q_ASSERT(m_account.isNull());
}
