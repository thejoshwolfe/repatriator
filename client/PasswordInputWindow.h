#ifndef PASSWORDINPUTWINDOW_H
#define PASSWORDINPUTWINDOW_H

#include <QDialog>
#include <QString>

namespace Ui {
    class PasswordInputWindow;
}

class PasswordInputWindow : public QDialog
{
    Q_OBJECT
public:
    struct Result {
        QString username;
        QString password;
    };

public:
    ~PasswordInputWindow();
    static PasswordInputWindow * instance();
    Result showGetPassword(QString dialog_title, QString ok_text, QString username);
protected:
    void changeEvent(QEvent *e);

private:
    explicit PasswordInputWindow(QWidget *parent = 0);
    Ui::PasswordInputWindow *ui;
    static PasswordInputWindow * s_instance;
    Result m_return_result;

private slots:
    void handleRejected();
    void handleAccepted();
};

#endif // PASSWORDINPUTWINDOW_H
