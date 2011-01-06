#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include "ConnectionSettings.h"
#include "Server.h"
#include "ServerTypes.h"
#include "IncomingMessage.h"
#include "Connector.h"

#include <QDialog>
#include <QSharedPointer>
#include <QHash>

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

    class DetailedUserInfo : public ServerTypes::UserInfo {
    public:
        enum ChangedStatus {
            Unchanged,
            New,
            Updated,
            Deleted,
        };

        QString password;
        ChangedStatus changed_status;

        DetailedUserInfo() :
            password(""),
            changed_status(Unchanged)
        {}

        DetailedUserInfo(ServerTypes::UserInfo & source) :
            ServerTypes::UserInfo(source.username, source.permissions),
            password(""),
            changed_status(Unchanged)
        {}
    };

    QHash<QString, QSharedPointer<DetailedUserInfo> > m_users;

    // whether we've received the user list from the server yet
    bool m_got_users;

private:
    void cleanup();
    void updateUserList(QList<ServerTypes::UserInfo> users);
    void enableCorrectControls();

private slots:
    void on_newButton_clicked();
    void on_deleteButton_clicked();
    void on_usersList_itemSelectionChanged();
    void on_changePasswordButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void handleAccepted();
    void handleRejected();

    void connected(QSharedPointer<Server> server);
    void connectionFailure(Connector::FailureReason reason);
    void processMessage(QSharedPointer<IncomingMessage> msg);
    void connectionEnded();
};

#endif // ADMINWINDOW_H
