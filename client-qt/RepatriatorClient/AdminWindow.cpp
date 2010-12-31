#include "AdminWindow.h"
#include "ui_AdminWindow.h"

#include "Connector.h"

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

void AdminWindow::showAdmin(ConnectionSettings *connection)
{
    Connector * connector = Connector::create(connection, false);

    bool success;
    success = connect(connector, SIGNAL(failure(int)), this, SLOT(connectionFailure(int)));
    Q_ASSERT(success);
    success = connect(connector, SIGNAL(success(QSharedPointer<Server>)), this, SLOT(connected(QSharedPointer<Server>)));
    Q_ASSERT(success);

    connector->go();

    this->exec();
}

void AdminWindow::on_buttonBox_rejected()
{
    cleanup();
}

void AdminWindow::connected(QSharedPointer<Server> server)
{
    m_server = server;
}

void AdminWindow::connectionFailure(int reason)
{
    Q_UNUSED(reason);
    reject();
}

void AdminWindow::cleanup()
{
    m_server.clear();
}

void AdminWindow::on_buttonBox_accepted()
{
    // apply changes


    cleanup();
}
