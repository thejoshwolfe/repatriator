#include "PasswordInputWindow.h"
#include "ui_PasswordInputWindow.h"

PasswordInputWindow * PasswordInputWindow::s_instance = NULL;

PasswordInputWindow::PasswordInputWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordInputWindow)
{
    ui->setupUi(this);
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

QString PasswordInputWindow::showGetPassword(QString dialog_title, QString ok_text, QString username)
{
    this->setWindowTitle(dialog_title);

    ui->buttonBox->clear();
    ui->buttonBox->addButton(ok_text, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(QDialogButtonBox::Cancel);

    ui->usernameLabel->setText(username);
    ui->passwordLineEdit->setText("");
    ui->passwordLineEdit->setFocus(Qt::OtherFocusReason);
    this->exec();
    return m_return_password;
}

void PasswordInputWindow::on_buttonBox_accepted()
{
    m_return_password = ui->passwordLineEdit->text();
}

void PasswordInputWindow::on_buttonBox_rejected()
{
    m_return_password = QString();
}
