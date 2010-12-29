#include "PasswordInputWindow.h"
#include "ui_PasswordInputWindow.h"

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
