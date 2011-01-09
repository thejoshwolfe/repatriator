#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
    class ChangePasswordDialog;
}

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT
public:

    struct Result {
        bool accepted;
        QString old_password;
        QString new_password;
    };

    static ChangePasswordDialog * instance();

    Result showChangePassword();

protected:
    void changeEvent(QEvent *e);

private:
    explicit ChangePasswordDialog(QWidget *parent = 0);
    ~ChangePasswordDialog();
    Ui::ChangePasswordDialog *ui;
    static ChangePasswordDialog * s_instance;
    Result m_result;
    void checkConfirmPassword();

private slots:
    void on_lineEditNewPassword_textChanged(QString );
    void on_lineEditConfirmNew_textChanged(QString );

    void handleRejected();
    void handleAccepted();
};

#endif // CHANGEPASSWORDDIALOG_H
