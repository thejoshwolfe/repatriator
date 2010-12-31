#ifndef EDITCONNECTIONWINDOW_H
#define EDITCONNECTIONWINDOW_H

#include <QDialog>

namespace Ui {
    class EditConnectionWindow;
}

class ConnectionSettings;

class EditConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    ~EditConnectionWindow();

    static EditConnectionWindow * instance();

    // show this window modally in "new" mode. you're still responsible
    // for creating and destroying connection.
    // returns whether the dialog box was accepted
    bool showNew(ConnectionSettings * connection);
    // show this window modally and edit passed in connection in "edit" mode
    // returns whether the dialog box was accepted
    bool showEdit(ConnectionSettings * connection);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EditConnectionWindow *ui;
    static EditConnectionWindow * s_instance;

    enum Mode {
        NewMode,
        EditMode,
    };

    Mode m_mode;

    ConnectionSettings * m_connection;

    static const char * c_static_password;

    bool m_accepted;
private:
    explicit EditConnectionWindow(QWidget *parent = 0);
    void enableCorrectControls();
    void updateConnectionWithControls();

private slots:
    void on_savePasswordCheckBox_toggled(bool checked);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void delegateFocusEvent(QWidget * from, QWidget * to);
    void passwordLineEdit_gotFocus();
};

#endif // EDITCONNECTIONWINDOW_H
