#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConnectionSettings.h"
#include "Server.h"
#include "Connector.h"
#include "ShadowSlider.h"

#include <QMainWindow>
#include <QSharedPointer>
#include <QProgressDialog>
#include <QVector>
#include <QMenu>
#include <QHash>

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
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

private:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    static MainWindow * s_instance;
    QSharedPointer<Server> m_server;
    QSharedPointer<Connector> m_connector;
    QProgressDialog * m_progressDialog;
    ConnectionSettings * m_connection_settings;

    QString m_quit_after_this_file;


    int m_next_download_number;
    // set this to true if we are expecting the server to close the connection
    bool m_quit_after_close;
    QMenu * m_pictures_context_menu;

    // for batch download
    qint64 m_bytes_done;
    qint64 m_bytes_total;
    int m_expected_download_count;

    QHash<QString, ServerTypes::DirectoryItem> m_file_info;

    QVector<ServerTypes::Bookmark> m_static_bookmarks;

private:
    void cleanup();
    void enableCorrectControls();
    void updateDirectoryList(QList<ServerTypes::DirectoryItem> items);
    void updateShadowPosition(ShadowSlider * slider, qint8 motor_state, qint64 motor_position);
    void updateShadowPositions(QVector<qint8> motor_states, QVector<qint64> motor_positions);
    void saveFile(QByteArray blob, QString remote_filename);
    bool checkDownloadDirectory();
    void sendTargetMotorPositions();
    void changeMotorBounds(QVector<InitInfoMessage::MotorBoundaries> motor_boundaries, ServerTypes::Bookmark home_location);
    void setLocations(QVector<ServerTypes::Bookmark> bookmarks);
    ServerTypes::Bookmark get_home_location_from_bookmarks(QVector<ServerTypes::Bookmark> bookmarks);
    void maybeSetSlider(ShadowSlider * slider, qint64 motor_position);
    void goToBookmark(ServerTypes::Bookmark bookmark);
    QString getNextDownloadFilename();
    void requestDownloadFile(QString remote_filename);

private slots:
    void on_autoFocusEnabledCheckBox_clicked(bool checked);
    void on_picturesList_customContextMenuRequested(QPoint pos);
    void on_orbitSliderA_valueChanged(int value);
    void on_orbitSliderB_valueChanged(int value);
    void on_liftSliderZ_valueChanged(int value);
    void on_picturesList_itemSelectionChanged();
    void on_actionDiscardSelectedFiles_triggered();
    void on_actionDownloadAllAndQuit_triggered();
    void on_actionDownload_triggered();
    void on_actionChangeDownloadFolder_triggered();
    void on_actionTakeSnapshot_triggered();
    void on_snapshotButton_clicked();
    void on_shadowMinimap_positionChosen(QPoint);
    void on_location_button_clicked();


    void connected();
    void connectionFailure(Connector::FailureReason reason);

    void processMessage(QSharedPointer<IncomingMessage> msg);
    void connectionEnded();
    void showProgress(qint64 bytes_done, qint64 bytes_total, IncomingMessage * msg);

    void showPing(int ms);
};

#endif // MAINWINDOW_H
