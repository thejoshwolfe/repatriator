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
    static ConnectionWindow * instance();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);

private:
    Ui::ConnectionWindow *ui;

    static ConnectionWindow * s_instance;

private:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();
    void refreshConnections();
    void enableCorrectControls();

private slots:
    void on_connectionTable_cellDoubleClicked(int row, int column);
    void on_connectionTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_loginButton_clicked();
    void on_deleteButton_clicked();
    void on_editButton_clicked();
    void on_newButton_clicked();
    void on_adminButton_clicked();
    void on_cancelButton_clicked();

};

#endif // CONNECTIONWINDOW_H
