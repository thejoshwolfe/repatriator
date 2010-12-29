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
    explicit EditUserAccountWindow(QWidget *parent = 0);
    ~EditUserAccountWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditUserAccountWindow *ui;
};

#endif // EDITUSERACCOUNTWINDOW_H
