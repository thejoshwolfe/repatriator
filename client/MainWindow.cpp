#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ConnectionWindow.h"
#include "Settings.h"
#include "EditBookmarkDialog.h"
#include "ChangePasswordDialog.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

MainWindow * MainWindow::s_instance = NULL;
const float MainWindow::c_lowest_sensitivity = 0.01f;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_progressDialog(new QProgressDialog(this, Qt::Dialog)),
    m_next_download_number(1),
    m_quit_after_close(false),
    m_bytes_done(0),
    m_bytes_total(0),
    m_expected_download_count(0),
    m_current_sensitivity(1.0f),
    m_motor_states(5)
{
    for (int i = 0; i < 5; i++)
        m_motor_states.replace(i, false);

    ui->setupUi(this);

    this->setCentralWidget(ui->displayWidget);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

    // create the context menu for file download box
    m_pictures_context_menu = new QMenu(this);
    m_pictures_context_menu->addAction(ui->actionSelectAll);
    m_pictures_context_menu->addSeparator();
    m_pictures_context_menu->addAction(ui->actionDownload);
    m_pictures_context_menu->addAction(ui->actionDiscardSelectedFiles);
    m_pictures_context_menu->addSeparator();
    m_pictures_context_menu->addAction(ui->actionDownloadAllAndQuit);

    // create the show/hide dock menu items in View
    ui->viewMenu->addAction(ui->controlsDock->toggleViewAction());
    ui->viewMenu->addAction(ui->filesDock->toggleViewAction());
    ui->viewMenu->addAction(ui->bookmarksDock->toggleViewAction());
    ui->viewMenu->addAction(ui->locationsDock->toggleViewAction());

    m_progressDialog->setWindowModality(Qt::NonModal);
    m_progressDialog->setWindowTitle(tr("Receiving data"));
    m_progressDialog->setMinimumDuration(3000);

    // configure the mapping between sensitivity slider values and percentage
    Q_ASSERT(ui->sensitivitySlider->minimum() == 0);
    float percent_range = (1.0f - c_lowest_sensitivity);
    float percent_delta = percent_range / (float) ui->sensitivitySlider->maximum();
    m_sensitivities.resize(ui->sensitivitySlider->maximum()+1);
    for (int i = 0; i <= ui->sensitivitySlider->maximum(); i++)
        m_sensitivities.replace(i, 1.0f - i * percent_delta);
    ui->sensitivitySlider->setValue(0);

    bool success;
    success = connect(ui->displayWidget, SIGNAL(focusPointChanged(QPointF)), this, SLOT(sendFocusPoint(QPointF)));
    Q_ASSERT(success);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

MainWindow * MainWindow::instance()
{
    if (! s_instance)
        s_instance = new MainWindow();
    return s_instance;
}

void MainWindow::showWithConnection(ConnectionSettings *connection)
{
    m_connection_settings = connection;

    enableCorrectControls();

    m_server = QSharedPointer<Server>(new Server(*connection));
    m_server.data()->setNeedHardware(true);
    m_connector = QSharedPointer<Connector>(new Connector(m_server));

    bool success;
    success = connect(m_connector.data(), SIGNAL(failure(Connector::FailureReason)), this, SLOT(connectionFailure(Connector::FailureReason)), Qt::QueuedConnection);
    Q_ASSERT(success);
    success = connect(m_connector.data(), SIGNAL(success()), this, SLOT(connected()));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SLOT(processMessage(QSharedPointer<IncomingMessage>)));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(progress(qint64,qint64,IncomingMessage*)), this, SLOT(showProgress(qint64,qint64,IncomingMessage*)));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(pingComputed(int)), this, SLOT(showPing(int)));
    Q_ASSERT(success);

    m_connector.data()->go();

    this->show();

    this->restoreState(Settings::main_window_state);
    this->restoreGeometry(Settings::main_window_geometry);
    ui->controlsDock->restoreGeometry(Settings::dock_controls_geometry);
    ui->filesDock->restoreGeometry(Settings::dock_files_geometry);
    ui->bookmarksDock->restoreGeometry(Settings::dock_bookmarks_geometry);
    ui->locationsDock->restoreGeometry(Settings::dock_locations_geometry);

    ui->displayWidget->clear();
}

void MainWindow::connected()
{
    bool success;
    success = connect(m_server.data(), SIGNAL(socketDisconnected()), this, SLOT(connectionEnded()));
    Q_ASSERT(success);

    m_connector.clear();
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new DirectoryListingRequestMessage()));

    enableCorrectControls();
}

void MainWindow::connectionFailure(Connector::FailureReason reason)
{
    Q_UNUSED(reason);

    m_connector.clear();
    ConnectionWindow::instance()->show();
    this->hide();
}

void MainWindow::connectionEnded()
{
    if (m_quit_after_close) {
        this->close();
        cleanup();
    } else {
        // unexpected
        QMessageBox::warning(this, tr("Server Closed Connection"),
            tr("The remote server closed the connection. You'll have to reconnect if you weren't done."), QMessageBox::Ok);
        ConnectionWindow::instance()->show();
        this->close();
        cleanup();
    }
}

void MainWindow::processMessage(QSharedPointer<IncomingMessage> msg)
{
    switch (msg.data()->type) {
        case IncomingMessage::FullUpdate:
        {
            FullUpdateMessage * full_update_msg = (FullUpdateMessage *) msg.data();
            ui->displayWidget->prepareDisplayImage(full_update_msg->image);
            updateMotorPositions(full_update_msg->motor_states, full_update_msg->motor_positions);
            break;
        }
        case IncomingMessage::ErrorMessage:
        {
            ErrorMessage * err_msg = (ErrorMessage *) msg.data();
            handleErrorMessage(err_msg->number, err_msg->message);
            break;
        }
        case IncomingMessage::DirectoryListingResult:
        {
            DirectoryListingResultMessage * directory_listing_msg = (DirectoryListingResultMessage *) msg.data();
            updateDirectoryList(directory_listing_msg->directory_list);
            break;
        }
        case IncomingMessage::FileDownloadResult:
        {
            FileDownloadResultMessage * file_download_msg = (FileDownloadResultMessage *) msg.data();
            saveFile(file_download_msg->file, file_download_msg->filename);
            break;
        }
        case IncomingMessage::InitInfo:
        {
            InitInfoMessage * init_info_msg = (InitInfoMessage *) msg.data();

            m_static_bookmarks = init_info_msg->static_bookmarks;
            refreshLocations(false);

            m_user_bookmarks = init_info_msg->user_bookmarks;
            ui->bookmarksList->clear();
            foreach (ServerTypes::Bookmark bookmark, m_user_bookmarks)
                ui->bookmarksList->addItem(bookmark.name);

            ServerTypes::Bookmark home_location = getHomeLocationFromBookmarks(init_info_msg->static_bookmarks);
            changeMotorBounds(init_info_msg->motor_boundaries, home_location);
            break;
        }
        default:
            qDebug() << "wtf got a message " << msg.data()->type;
            Q_ASSERT(false);
    }
}

void MainWindow::handleErrorMessage(ErrorMessage::ErrorType type, QString msg)
{
    qDebug() << "Incoming error message:" << type << ":" << msg;
    switch (type) {
    case ErrorMessage::BadPassword:
        QMessageBox::warning(this, tr("Invalid Password"), tr("The password you used was invalid."), QMessageBox::Ok);
        break;
    default:
        break;
    }
}

void MainWindow::updateDirectoryList(QList<ServerTypes::DirectoryItem> items)
{
    ui->picturesList->clear();
    foreach (ServerTypes::DirectoryItem item, items) {
        ui->picturesList->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(item.thumbnail)), item.filename));
        m_file_info.insert(item.filename, item);
    }
    enableCorrectControls();
}

void MainWindow::updateMotorPositions(QVector<qint8> motor_states, QVector<qint64> motor_positions)
{
    m_motor_states = motor_states;
    ui->orbitSliderA->setShadowPosition((int)motor_positions.at(0));
    ui->orbitSliderB->setShadowPosition((int)motor_positions.at(1));
    ui->liftSliderZ->setShadowPosition((int)motor_positions.at(4));

    ui->shadowMinimap->setShadowPosition(QPoint((int)motor_positions.at(2), (int)motor_positions.at(3)));

    enableCorrectControls();
}

void MainWindow::saveFile(QByteArray blob, QString remote_filename)
{
    m_expected_download_count--;
    m_bytes_done += blob.size();
    if (m_expected_download_count <= 0) {
        m_expected_download_count = 0;
        m_bytes_done = 0;
        m_bytes_total = 0;
        m_progressDialog->reset();
    }

    QDir downloadDir(Settings::download_directory);
    if (! downloadDir.exists())
        return;
    QString local_filename = getNextDownloadFilename();
    QFile downloadFile(downloadDir.absoluteFilePath(local_filename));
    downloadFile.open(QFile::WriteOnly);
    downloadFile.write(blob);
    downloadFile.close();

    // successful download, discard server copy
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new FileDeleteRequestMessage(remote_filename)));

    if (remote_filename == m_quit_after_this_file) {
        m_quit_after_close = true;
        m_server.data()->finishWritingAndDisconnect();
    }
}

void MainWindow::cleanup()
{
    m_server.clear();
}

void MainWindow::showProgress(qint64 bytes_done, qint64 bytes_total, IncomingMessage *msg)
{
    Q_UNUSED(bytes_total);
    if (msg->type == IncomingMessage::FileDownloadResult && m_expected_download_count > 0)
        m_progressDialog->setValue((int)(m_bytes_done + bytes_done));
}

void MainWindow::enableCorrectControls()
{
    bool server_connected = ! m_server.isNull() && (m_server.data()->loginStatus() == ServerTypes::Success);
    bool pictures_list_nonempty = ui->picturesList->count() > 0;
    bool a_picture_is_selected = ui->picturesList->selectedItems().count() > 0;
    bool a_bookmark_is_selected = ui->bookmarksList->selectedItems().count() > 0;

    ui->actionTakeSnapshot->setEnabled(server_connected);
    ui->snapshotButton->setEnabled(server_connected);
    ui->newBookmarkButton->setEnabled(server_connected);

    ui->actionDownloadAllAndQuit->setEnabled(pictures_list_nonempty && server_connected);
    ui->actionSelectAll->setEnabled(pictures_list_nonempty && server_connected);

    ui->actionDownload->setEnabled(a_picture_is_selected && server_connected);
    ui->actionDiscardSelectedFiles->setEnabled(a_picture_is_selected && server_connected);

    ui->shadowMinimap->setEnabled(server_connected && m_motor_states.at(2) && m_motor_states.at(3));
    ui->liftSliderZ->setEnabled(server_connected && m_motor_states.at(4));
    ui->orbitSliderA->setEnabled(server_connected && m_motor_states.at(0));
    ui->orbitSliderB->setEnabled(server_connected && m_motor_states.at(1));

    ui->goToBookmarkButton->setEnabled(server_connected && a_bookmark_is_selected);
    ui->editBookmarkButton->setEnabled(server_connected && a_bookmark_is_selected);
    ui->deleteBookmarkButton->setEnabled(server_connected && a_bookmark_is_selected);
}

void MainWindow::on_snapshotButton_clicked()
{
    on_actionTakeSnapshot_triggered();
}

void MainWindow::on_actionTakeSnapshot_triggered()
{
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new TakePictureMessage()));
}

void MainWindow::on_actionChangeDownloadFolder_triggered()
{
    QString result = QFileDialog::getExistingDirectory(this, QString(), Settings::download_directory);
    if (result.isEmpty())
        return;
    Settings::download_directory = result;
    Settings::save();
}

bool MainWindow::checkDownloadDirectory()
{
    // make sure we have a download dir
    if (! Settings::download_directory.isEmpty())
        return true;
    QString result = QFileDialog::getExistingDirectory(this);
    if (result.isEmpty())
        return false;
    Settings::download_directory = result;
    Settings::save();
    return true;
}

void MainWindow::on_actionDownload_triggered()
{
    if (! checkDownloadDirectory())
        return;
    // send download file messages for each file
    for (int i = 0; i < ui->picturesList->selectedItems().count(); i++) {
        QString filename = ui->picturesList->selectedItems().at(i)->text();
        requestDownloadFile(filename);
    }
}

void MainWindow::on_actionDownloadAllAndQuit_triggered()
{
    // send download file messages for each file
    for (int i = 0; i < ui->picturesList->count(); i++) {
        QString remote_filename = ui->picturesList->item(i)->text();
        requestDownloadFile(remote_filename);
        m_quit_after_this_file = remote_filename;
    }
}

void MainWindow::requestDownloadFile(QString remote_filename)
{
    m_bytes_total += m_file_info.value(remote_filename).byte_count;
    m_progressDialog->setRange(0, (int)m_bytes_total);
    m_expected_download_count += 1;
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new FileDownloadRequestMessage(remote_filename)));
}

void MainWindow::on_actionDiscardSelectedFiles_triggered()
{
    // send delete file messages for each file
    for (int i = 0; i < ui->picturesList->selectedItems().count(); i++) {
        QString filename = ui->picturesList->selectedItems().at(i)->text();
        m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new FileDeleteRequestMessage(filename)));
    }
}

void MainWindow::on_picturesList_itemSelectionChanged()
{
    enableCorrectControls();
}

void MainWindow::on_shadowMinimap_positionChosen(QPoint)
{
    sendTargetMotorPositions();
}

void MainWindow::sendTargetMotorPositions()
{
    QVector<qint64> m_target_motor_positions(5);
    m_target_motor_positions[0] = ui->orbitSliderA->value();
    m_target_motor_positions[1] = ui->orbitSliderB->value();
    m_target_motor_positions[2] = ui->shadowMinimap->position().x();
    m_target_motor_positions[3] = ui->shadowMinimap->position().y();
    m_target_motor_positions[4] = ui->liftSliderZ->value();

    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new MotorMovementMessage(m_target_motor_positions)));
}

void MainWindow::on_liftSliderZ_valueChanged(int)
{
    sendTargetMotorPositions();
}

void MainWindow::on_orbitSliderB_valueChanged(int)
{
    sendTargetMotorPositions();
}

void MainWindow::on_orbitSliderA_valueChanged(int)
{
    sendTargetMotorPositions();
}

void MainWindow::changeMotorBounds(QVector<InitInfoMessage::MotorBoundaries> bounds, ServerTypes::Bookmark home_location)
{
    blockSliderSignals(true);

    ui->orbitSliderA->setMinimum((int)bounds.at(0).min);
    ui->orbitSliderA->setMaximum((int)bounds.at(0).max);

    ui->orbitSliderB->setMinimum((int)bounds.at(1).min);
    ui->orbitSliderB->setMaximum((int)bounds.at(1).max);

    ui->shadowMinimap->setMinPosition(QPoint((int)bounds.at(2).min, (int)bounds.at(3).min));
    ui->shadowMinimap->setMaxPosition(QPoint((int)bounds.at(2).max, (int)bounds.at(3).max));

    ui->liftSliderZ->setMinimum(bounds.at(4).min);
    ui->liftSliderZ->setMaximum(bounds.at(4).max);

    blockSliderSignals(false);

    goToBookmark(home_location);
    sendFocusPoint(ui->displayWidget->focusPoint());
}

void MainWindow::goToBookmark(ServerTypes::Bookmark bookmark)
{
    blockSliderSignals(true);
    bool moved = false;

    moved |= maybeSetSlider(ui->orbitSliderA, bookmark.motor_positions.at(0));
    moved |= maybeSetSlider(ui->orbitSliderB, bookmark.motor_positions.at(1));

    QPoint xy = ui->shadowMinimap->position();
    if (bookmark.motor_positions.at(2) != ServerTypes::MotorPositionNotSpecified) {
        xy.setX(bookmark.motor_positions.at(2));
        moved = true;
    }
    if (bookmark.motor_positions.at(3) != ServerTypes::MotorPositionNotSpecified) {
        xy.setY(bookmark.motor_positions.at(3));
        moved = true;
    }
    ui->shadowMinimap->setPosition(xy);

    moved |= maybeSetSlider(ui->liftSliderZ, bookmark.motor_positions.at(4));

    blockSliderSignals(false);

    if (moved)
        sendTargetMotorPositions();
}

void MainWindow::blockSliderSignals(bool value)
{
    ui->orbitSliderA->blockSignals(value);
    ui->orbitSliderB->blockSignals(value);
    ui->shadowMinimap->blockSignals(value);
    ui->liftSliderZ->blockSignals(value);
}

bool MainWindow::maybeSetSlider(ShadowSlider *slider, qint64 motor_position)
{
    if (motor_position == ServerTypes::MotorPositionNotSpecified)
        return false;
    if (motor_position == slider->value())
        return false;
    slider->setValue(motor_position);
    return true;
}

void MainWindow::refreshLocations(bool save)
{
    foreach (QObject * child, ui->locationsLayout->children())
        delete child;
    for (int i = 0; i < m_static_bookmarks.size(); i++) {
        ServerTypes::Bookmark bookmark = m_static_bookmarks.at(i);
        int readable_index = i + 1;
        QString text = "";
        if (readable_index <= 9)
            text = "&";
        text += QString::number(readable_index) + ". " + bookmark.name;
        QPushButton * location_button = new QPushButton(text);
        bool success = connect(location_button, SIGNAL(clicked()), this, SLOT(location_button_clicked()));
        Q_ASSERT(success);
        ui->locationsLayout->addWidget(location_button);
    }

    enableCorrectControls();

    if (save)
        m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new SetStaticBookmarksMessage(m_static_bookmarks)));
}
void MainWindow::location_button_clicked()
{
    QPushButton * location_button = (QPushButton *) sender();
    QString button_text = location_button->text();
    if (button_text.startsWith("&"))
        button_text = button_text.mid(1);
    int dot_index = button_text.indexOf('.');
    QString one_based_index_string = button_text.mid(0, dot_index);
    int one_based_index = one_based_index_string.toInt();
    ServerTypes::Bookmark bookmark = m_static_bookmarks.at(one_based_index - 1);
    goToBookmark(bookmark);
}

void MainWindow::saveBookmarks()
{
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new SetUserBookmarksMessage(m_user_bookmarks)));
}

ServerTypes::Bookmark MainWindow::getHomeLocationFromBookmarks(QList<ServerTypes::Bookmark> bookmarks)
{
    if (bookmarks.size() > 0)
        return bookmarks.at(0);
    // make up a home location
    ServerTypes::Bookmark home_location;
    home_location.name = "Home";
    for (int i = 0; i < 5; i++)
        home_location.motor_positions.append(0);
    home_location.auto_focus = ServerTypes::NotSpecified;
    return home_location;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    Settings::dock_controls_geometry = ui->controlsDock->saveGeometry();
    Settings::dock_files_geometry = ui->filesDock->saveGeometry();
    Settings::dock_bookmarks_geometry = ui->bookmarksDock->saveGeometry();
    Settings::dock_locations_geometry = ui->locationsDock->saveGeometry();

    Settings::main_window_geometry = this->saveGeometry();
    Settings::main_window_state = this->saveState();
    Settings::save();
}

QString MainWindow::getNextDownloadFilename()
{
    forever {
        QDir folder(Settings::download_directory);
        QString filename = folder.absoluteFilePath(QString("img_") + QString::number(m_next_download_number) + QString(".jpg"));
        if (! QFileInfo(filename).exists())
            return filename;
        m_next_download_number++;
    }
}

void MainWindow::on_picturesList_customContextMenuRequested(QPoint pos)
{
    m_pictures_context_menu->popup(ui->picturesList->mapToGlobal(pos));
}

void MainWindow::on_autoFocusEnabledCheckBox_clicked(bool checked)
{
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new SetAutoFocusEnabledMessage(checked)));
}

void MainWindow::showPing(int ms)
{
    ui->statusBar->showMessage(tr("Ping: ") + QString::number(ms));
}

void MainWindow::on_sensitivitySlider_valueChanged(int value)
{
    float new_sensitivity = m_sensitivities.value(value);
    m_current_sensitivity = new_sensitivity;
    ui->sensitivityLabel->setText(tr("S&ensitivity: %1%").arg(QString::number(qRound(new_sensitivity * 100))));
    updateControlSensitivities();
}

void MainWindow::updateControlSensitivities()
{
    ui->orbitSliderA->setSensitivity(m_current_sensitivity);
    ui->orbitSliderB->setSensitivity(m_current_sensitivity);
    ui->shadowMinimap->setSensitivity(m_current_sensitivity);
    ui->liftSliderZ->setSensitivity(m_current_sensitivity);
}

void MainWindow::on_goToBookmarkButton_clicked()
{
    int index = selectedBookmarkIndex();
    if (index == -1)
        return;
    goToBookmark(m_user_bookmarks.at(index));
}

int MainWindow::selectedBookmarkIndex()
{
    if (ui->bookmarksList->selectedItems().isEmpty())
        return -1;
    return ui->bookmarksList->row(ui->bookmarksList->selectedItems().first());
}

void MainWindow::on_bookmarksList_itemSelectionChanged()
{
    enableCorrectControls();
}

void MainWindow::on_bookmarkHereButton_clicked()
{
    ServerTypes::Bookmark new_bookmark = here();

    // determin a name for the new bookmark
    QSet<QString> existing_names;
    foreach (ServerTypes::Bookmark bookmark, m_user_bookmarks)
        existing_names.insert(bookmark.name);
    for (int number = 1; ; number++) {
        QString name = tr("Bookmark ") + QString::number(number);
        if (existing_names.contains(name))
            continue;
        new_bookmark.name = name;
        break;
    }

    m_user_bookmarks.append(new_bookmark);
    ui->bookmarksList->addItem(new_bookmark.name);

    saveBookmarks();
}

void MainWindow::on_deleteBookmarkButton_clicked()
{
    int index = selectedBookmarkIndex();
    if (index == -1)
        return;

    m_user_bookmarks.removeAt(index);
    delete ui->bookmarksList->item(index);

    saveBookmarks();
}

void MainWindow::sendFocusPoint(QPointF point)
{
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new ChangeFocusLocationMessage(point)));
}

void MainWindow::on_editBookmarkButton_clicked()
{
    int index = selectedBookmarkIndex();
    if (index == -1)
        return;

    ServerTypes::Bookmark bookmark = m_user_bookmarks.at(index);
    if (!EditBookmarkDialog::showEdit(&bookmark, here()))
        return;

    m_user_bookmarks.replace(index, bookmark);
    ui->bookmarksList->item(index)->setText(bookmark.name);

    saveBookmarks();
}

ServerTypes::Bookmark MainWindow::here()
{
    ServerTypes::Bookmark bookmark;
    // no name
    bookmark.motor_positions.append(ui->orbitSliderA->value());
    bookmark.motor_positions.append(ui->orbitSliderB->value());
    bookmark.motor_positions.append(ui->shadowMinimap->position().x());
    bookmark.motor_positions.append(ui->shadowMinimap->position().y());
    bookmark.motor_positions.append(ui->liftSliderZ->value());
    bookmark.auto_focus = ServerTypes::NotSpecified;

    return bookmark;
}

void MainWindow::on_actionChangePassword_triggered()
{
    ChangePasswordDialog::Result result = ChangePasswordDialog::instance()->showChangePassword();
    if (! result.accepted)
        return;
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new ChangePasswordRequestMessage(result.old_password, result.new_password)));
}
