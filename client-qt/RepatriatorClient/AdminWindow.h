#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include "ConnectionSettings.h"
#include "Server.h"

#include <QDialog>
#include <QSharedPointer>

namespace Ui {
    class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    ~AdminWindow();

    static AdminWindow * instance();

    void showAdmin(ConnectionSettings * connection);

protected:
    void changeEvent(QEvent *e);

private:
    explicit AdminWindow(QWidget *parent = 0);
    Ui::AdminWindow *ui;
    static AdminWindow * s_instance;

    QSharedPointer<Server> m_server;

private:
    void cleanup();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void connected(QSharedPointer<Server> server);
    void connectionFailure(int reason);
};

#endif // ADMINWINDOW_H
