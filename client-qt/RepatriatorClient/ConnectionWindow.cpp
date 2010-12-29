#include "ConnectionWindow.h"
#include "ui_ConnectionWindow.h"

#include "AdminWindow.h"

ConnectionWindow * ConnectionWindow::s_instance = NULL;

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::changeEvent(QEvent *e)
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

ConnectionWindow * ConnectionWindow::instance()
{
    if (! s_instance)
        s_instance = new ConnectionWindow();
    return s_instance;
}

void ConnectionWindow::on_cancelButton_clicked()
{
    this->close();
}

void ConnectionWindow::on_adminButton_clicked()
{
    AdminWindow::instance()->show();
}
