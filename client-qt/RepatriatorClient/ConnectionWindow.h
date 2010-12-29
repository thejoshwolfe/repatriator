#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QDialog>

namespace Ui {
    class ConnectionWindow;
}

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();

    static ConnectionWindow * instance();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConnectionWindow *ui;

    static ConnectionWindow * s_instance;

private slots:
    void on_adminButton_clicked();
    void on_cancelButton_clicked();
};

#endif // CONNECTIONWINDOW_H
