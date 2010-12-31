#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ConnectionWindow.h"
#include "Connector.h"

MainWindow * MainWindow::s_instance = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

MainWindow * MainWindow::instance()
{
    if (! s_instance)
        s_instance = new MainWindow();
    return s_instance;
}

void MainWindow::showWithConnection(ConnectionSettings *connection)
{
    Connector * connector = Connector::create(connection, true);

    bool success;
    success = connect(connector, SIGNAL(failure(int)), this, SLOT(connectionFailure(int)));
    Q_ASSERT(success);
    success = connect(connector, SIGNAL(success(QSharedPointer<Server>)), this, SLOT(connected(QSharedPointer<Server>)));
    Q_ASSERT(success);

    connector->go();

    this->show();
}

void MainWindow::connected(QSharedPointer<Server> server)
{
    m_server = server;
}

void MainWindow::connectionFailure(int reason)
{
    Q_UNUSED(reason);
    ConnectionWindow::instance()->show();
    this->hide();
}
