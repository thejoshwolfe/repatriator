#ifndef EDITCONNECTIONWINDOW_H
#define EDITCONNECTIONWINDOW_H

#include <QDialog>

namespace Ui {
    class EditConnectionWindow;
}

class EditConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditConnectionWindow(QWidget *parent = 0);
    ~EditConnectionWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditConnectionWindow *ui;
};

#endif // EDITCONNECTIONWINDOW_H
