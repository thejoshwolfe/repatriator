#ifndef EDITUSERACCOUNTWINDOW_H
#define EDITUSERACCOUNTWINDOW_H

#include <QDialog>

namespace Ui {
    class EditUserAccountWindow;
}

class EditUserAccountWindow : public QDialog
{
    Q_OBJECT

public:
    class UserAccount {
    public:
        QString username;
        QString password;
        bool is_admin;
    };

public:
    static EditUserAccountWindow * instance();

    QSharedPointer<UserAccount> showGetNewUser();
    void showEditUser(QSharedPointer<UserAccount> account);

protected:
    void changeEvent(QEvent *e);

private:
    explicit EditUserAccountWindow(QWidget *parent = 0);
    ~EditUserAccountWindow();
    Ui::EditUserAccountWindow *ui;
    static EditUserAccountWindow * s_instance;

    QSharedPointer<UserAccount> m_account;

private slots:
    void handleRejected();
    void handleAccepted();
};

#endif // EDITUSERACCOUNTWINDOW_H
