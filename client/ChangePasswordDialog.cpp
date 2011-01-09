#include "ChangePasswordDialog.h"
#include "ui_ChangePasswordDialog.h"

#include <QPushButton>

ChangePasswordDialog * ChangePasswordDialog::s_instance = NULL;

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePasswordDialog)
{
    ui->setupUi(this);
    QPalette palette = ui->labelConfirmMatch->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->labelConfirmMatch->setPalette(palette);

    bool success;
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete ui;
}

ChangePasswordDialog * ChangePasswordDialog::instance()
{
    if (s_instance == NULL)
        s_instance = new ChangePasswordDialog();
    return s_instance;
}

void ChangePasswordDialog::changeEvent(QEvent *e)
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

ChangePasswordDialog::Result ChangePasswordDialog::showChangePassword()
{
    ui->lineEditConfirmNew->clear();
    ui->lineEditNewPassword->clear();
    ui->lineEditOldPassword->clear();
    checkConfirmPassword();

    ui->lineEditOldPassword->setFocus(Qt::OtherFocusReason);

    this->exec();

    return m_result;
}

void ChangePasswordDialog::handleAccepted()
{
    m_result.accepted = true;
    m_result.old_password = ui->lineEditOldPassword->text();
    m_result.new_password = ui->lineEditNewPassword->text();
}

void ChangePasswordDialog::handleRejected()
{
    m_result.accepted = false;
}

void ChangePasswordDialog::on_lineEditConfirmNew_textChanged(QString )
{
    checkConfirmPassword();
}

void ChangePasswordDialog::on_lineEditNewPassword_textChanged(QString )
{
    checkConfirmPassword();
}

void ChangePasswordDialog::checkConfirmPassword()
{
    bool match = (ui->lineEditNewPassword->text() == ui->lineEditConfirmNew->text());
    ui->labelConfirmMatch->setVisible(! match && ! ui->lineEditConfirmNew->text().isEmpty());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(match && ! ui->lineEditNewPassword->text().isEmpty());
}
