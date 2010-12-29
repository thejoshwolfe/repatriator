#include "AdminWindow.h"
#include "ui_AdminWindow.h"

AdminWindow * AdminWindow::s_instance = NULL;

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
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

void AdminWindow::on_buttonBox_rejected()
{
    this->close();
}
