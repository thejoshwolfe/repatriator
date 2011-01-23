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
#include <QListWidgetItem>

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

    QVector<ServerTypes::MotorBoundaries> m_motor_bounds;
    QList<ServerTypes::Bookmark> m_static_bookmarks;
    QList<ServerTypes::Bookmark> m_user_bookmarks;
    QList<QPushButton *> m_location_buttons;

    static const float c_lowest_sensitivity;
    QVector<float> m_sensitivities;
    float m_current_sensitivity;

    QVector<qint8> m_motor_states;

    struct Compensation {
        float value;
        QString label;
        Compensation() {}
        Compensation(float value, QString label) : value(value), label(label) {}
    };

    QVector<Compensation> m_compensation_values;

private:
    void cleanup();
    void enableCorrectControls();
    void updateDirectoryList(QList<ServerTypes::DirectoryItem> items);
    void updateMotorPositions(QVector<qint8> motor_states, QVector<qint64> motor_positions);
    void saveFile(QByteArray blob, QString remote_filename);
    bool checkDownloadDirectory();
    void sendTargetMotorPositions();
    void updateMotorBoundsWidgets();
    void refreshLocations(bool save);
    void saveBookmarks();
    ServerTypes::Bookmark getHomeLocationFromBookmarks(QList<ServerTypes::Bookmark> bookmarks);
    bool maybeSetSlider(ShadowSlider * slider, qint64 motor_position);
    void blockSliderSignals(bool value);
    void goToBookmark(ServerTypes::Bookmark bookmark);
    QString getNextDownloadFilename();
    void requestDownloadFile(QString remote_filename);
    void updateControlSensitivities();
    int selectedBookmarkIndex();
    ServerTypes::Bookmark here();
    ServerTypes::Bookmark newBookmarkHere();
    void handleErrorMessage(ErrorMessage::ErrorType type, QString msg);
    QList<ServerTypes::Bookmark> * getSupposedUserBookmarks();
    bool isAdmin();

private slots:
    void on_actionAbout_triggered();
    void on_actionChang_Motor_Bounds_triggered();
    void on_exposureCompensationSlider_valueChanged(int value);
    void on_newBookmarkButton_clicked();
    void on_actionChangePassword_triggered();
    void on_editBookmarkButton_clicked();
    void on_deleteBookmarkButton_clicked();
    void on_bookmarksList_itemSelectionChanged();
    void on_bookmarkHereButton_clicked();
    void on_sensitivitySlider_valueChanged(int value);
    void on_goToBookmarkButton_clicked();
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
    void location_button_clicked();

    void connected();
    void connectionFailure(Connector::FailureReason reason);

    void processMessage(QSharedPointer<IncomingMessage> msg);
    void connectionEnded();
    void showProgress(qint64 bytes_done, qint64 bytes_total, IncomingMessage * msg);

    void showPing(int ms);
    void sendFocusPoint(QPointF point);
};

#endif // MAINWINDOW_H
