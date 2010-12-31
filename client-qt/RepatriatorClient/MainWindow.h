#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConnectionSettings.h"
#include "Server.h"
#include "Connector.h"

#include <QMainWindow>
#include <QSharedPointer>
#include <QProgressDialog>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow * instance();

    void showWithConnection(ConnectionSettings * connection);

protected:
    void changeEvent(QEvent *e);

private:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    static MainWindow * s_instance;
    QSharedPointer<Server> m_server;
    QProgressDialog * m_progressDialog;
    ConnectionSettings * m_connection_settings;

    QString m_quit_after_this_file;

private:
    void cleanup();
    void enableCorrectControls();
    void updateDirectoryList(QList<ServerTypes::DirectoryItem> items);
    void updateShadowPositions(QVector<qint64> motor_positions);
    void saveFile(QByteArray blob, QString filename);
    bool checkDownloadDirectory();

private slots:
    void on_picturesList_itemSelectionChanged();
    void on_actionDiscardSelectedFiles_triggered();
    void on_actionDownloadAllAndQuit_triggered();
    void on_actionDownload_triggered();
    void on_actionChangeDownloadFolder_triggered();
    void on_actionTakeSnapshot_triggered();
    void on_snapshotButton_clicked();
    void connected(QSharedPointer<Server> server);
    void connectionFailure(Connector::FailureReason reason);

    void processMessage(QSharedPointer<IncomingMessage> msg);
    void connectionEnded();
    void showProgress(qint64 bytes_done, qint64 bytes_total, IncomingMessage * msg);

};

#endif // MAINWINDOW_H
