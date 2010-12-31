#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConnectionSettings.h"
#include "Server.h"
#include "Connector.h"

#include <QMainWindow>
#include <QSharedPointer>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow * instance();

    void showWithConnection(ConnectionSettings * connection);

protected:
    void changeEvent(QEvent *e);

private:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    static MainWindow * s_instance;
    QSharedPointer<Server> m_server;

private:

private slots:
    void connected(QSharedPointer<Server> server);
    void connectionFailure(Connector::FailureReason reason);

};

#endif // MAINWINDOW_H
