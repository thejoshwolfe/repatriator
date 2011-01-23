#include "PasswordInputWindow.h"
#include "ui_PasswordInputWindow.h"

PasswordInputWindow * PasswordInputWindow::s_instance = NULL;

PasswordInputWindow::PasswordInputWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordInputWindow)
{
    ui->setupUi(this);

    bool success;
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
}

PasswordInputWindow::~PasswordInputWindow()
{
    delete ui;
}

void PasswordInputWindow::changeEvent(QEvent *e)
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

PasswordInputWindow * PasswordInputWindow::instance()
{
    if (! s_instance)
        s_instance = new PasswordInputWindow();
    return s_instance;
}

PasswordInputWindow::Result PasswordInputWindow::showGetPassword(QString dialog_title, QString ok_text, QString username)
{
    this->setWindowTitle(dialog_title);

    ui->buttonBox->clear();
    ui->buttonBox->addButton(ok_text, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(QDialogButtonBox::Cancel);

    ui->usernameLineEdit->setText(username);
    ui->passwordLineEdit->setText("");
    if (username.isEmpty())
        ui->usernameLineEdit->setFocus(Qt::OtherFocusReason);
    else
        ui->passwordLineEdit->setFocus(Qt::OtherFocusReason);
    this->exec();
    return m_return_result;
}

void PasswordInputWindow::handleAccepted()
{
    m_return_result.username = ui->usernameLineEdit->text();
    m_return_result.password = ui->passwordLineEdit->text();
}

void PasswordInputWindow::handleRejected()
{
    m_return_result.username = QString();
    m_return_result.password = QString();
}
