#ifndef PASSWORDINPUTWINDOW_H
#define PASSWORDINPUTWINDOW_H

#include <QDialog>

namespace Ui {
    class PasswordInputWindow;
}

class PasswordInputWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordInputWindow(QWidget *parent = 0);
    ~PasswordInputWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PasswordInputWindow *ui;
};

#endif // PASSWORDINPUTWINDOW_H
