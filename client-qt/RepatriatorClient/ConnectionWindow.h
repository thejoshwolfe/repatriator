#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QDialog>
#include <QPushButton>

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
    void hideEvent(QHideEvent *);

private:
    Ui::ConnectionWindow *ui;

    static ConnectionWindow * s_instance;

    QPushButton * m_loginButton;

private:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();
    void refreshConnections();
    void enableCorrectControls();
    bool oneConnectionIsSelected() const;

private slots:
    void on_connectionTable_itemSelectionChanged();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_connectionTable_cellDoubleClicked(int row, int column);
    void on_deleteButton_clicked();
    void on_editButton_clicked();
    void on_newButton_clicked();
    void on_adminButton_clicked();

    void handleRejected();
    void handleAccepted();

};

#endif // CONNECTIONWINDOW_H
