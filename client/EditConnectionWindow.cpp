#include "EditConnectionWindow.h"
#include "ui_EditConnectionWindow.h"

#include "ConnectionSettings.h"

#include <QApplication>
#include <QWidget>

EditConnectionWindow * EditConnectionWindow::s_instance = NULL;
const char * EditConnectionWindow::c_static_password = "12345678";

EditConnectionWindow::EditConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditConnectionWindow)
{
    ui->setupUi(this);
    bool success;
    success = connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(delegateFocusEvent(QWidget*,QWidget*)));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
}

EditConnectionWindow::~EditConnectionWindow()
{
    delete ui;
}

void EditConnectionWindow::changeEvent(QEvent *e)
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

EditConnectionWindow * EditConnectionWindow::instance()
{
    if (! s_instance)
        s_instance = new EditConnectionWindow();
    return s_instance;
}

bool EditConnectionWindow::showNew(ConnectionSettings * connection)
{
    this->setWindowTitle(tr("New Connection - Repatriator"));
    m_mode = NewMode;
    m_connection = connection;

    // clear controls
    ui->addressLineEdit->setText("");
    ui->portLineEdit->setText("");
    ui->usernameLineEdit->setText("");
    ui->savePasswordCheckBox->setChecked(false);
    ui->passwordLineEdit->setText("");

    ui->addressLineEdit->setFocus(Qt::OtherFocusReason);

    enableCorrectControls();

    this->exec();
    return m_accepted;
}

bool EditConnectionWindow::showEdit(ConnectionSettings * connection)
{
    this->setWindowTitle(tr("Edit Connection - Repatriator"));
    m_mode = EditMode;
    m_connection = connection;

    // load values from connection
    ui->addressLineEdit->setText(connection->host);
    ui->portLineEdit->setText(QString::number(connection->port));
    ui->usernameLineEdit->setText(connection->username);
    bool save_password = connection->password.length() != 0;
    ui->savePasswordCheckBox->setChecked(save_password);
    ui->passwordLineEdit->setText(save_password ? c_static_password : "");

    ui->addressLineEdit->setFocus(Qt::OtherFocusReason);

    enableCorrectControls();

    this->exec();
    return m_accepted;
}

void EditConnectionWindow::enableCorrectControls()
{
    ui->passwordLineEdit->setEnabled(ui->savePasswordCheckBox->isChecked());
    ui->passwordLabel->setEnabled(ui->savePasswordCheckBox->isChecked());
}
void EditConnectionWindow::handleAccepted()
{
    updateConnectionWithControls();
    m_accepted = true;
}

void EditConnectionWindow::handleRejected()
{
    m_accepted = false;
}

void EditConnectionWindow::updateConnectionWithControls()
{
    m_connection->host = ui->addressLineEdit->text();
    m_connection->port = ui->portLineEdit->text().toInt();
    m_connection->username = ui->usernameLineEdit->text();
    if (ui->savePasswordCheckBox->isChecked() && ui->passwordLineEdit->text() != c_static_password)
        m_connection->password = ui->passwordLineEdit->text();
    else
        m_connection->password = QString();
}

void EditConnectionWindow::on_savePasswordCheckBox_toggled(bool checked)
{
    Q_UNUSED(checked);
    enableCorrectControls();
}

void EditConnectionWindow::delegateFocusEvent(QWidget *from, QWidget *to)
{
    Q_UNUSED(from);
    if (to == ui->passwordLineEdit)
        passwordLineEdit_gotFocus();
}

void EditConnectionWindow::passwordLineEdit_gotFocus()
{
    if (ui->passwordLineEdit->text() == c_static_password)
        ui->passwordLineEdit->setText(c_static_password);
}
