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
    ~PasswordInputWindow();
    static PasswordInputWindow * instance();
    QString showGetPassword(QString username);

protected:
    void changeEvent(QEvent *e);

private:
    explicit PasswordInputWindow(QWidget *parent = 0);
    Ui::PasswordInputWindow *ui;
    static PasswordInputWindow * s_instance;
    QString m_return_password;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // PASSWORDINPUTWINDOW_H
