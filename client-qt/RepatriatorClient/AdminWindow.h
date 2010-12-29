#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>

namespace Ui {
    class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = 0);
    ~AdminWindow();

    static AdminWindow * instance();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AdminWindow *ui;
    static AdminWindow * s_instance;

private slots:
    void on_buttonBox_rejected();
};

#endif // ADMINWINDOW_H
