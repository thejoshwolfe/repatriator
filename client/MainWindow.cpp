#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ConnectionWindow.h"
#include "Settings.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

MainWindow * MainWindow::s_instance = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_progressDialog(new QProgressDialog(this, Qt::Dialog)),
    m_next_download_number(1),
    m_quit_after_close(false),
    m_bytes_done(0),
    m_bytes_total(0),
    m_expected_download_count(0)
{
    m_progressDialog->setWindowModality(Qt::NonModal);
    m_progressDialog->setWindowTitle(tr("Receiving data"));
    m_progressDialog->setMinimumDuration(3000);
    ui->setupUi(this);

    this->setCentralWidget(ui->displayWidget);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    m_pictures_context_menu = new QMenu(this);
    m_pictures_context_menu->addAction(ui->actionSelectAll);
    m_pictures_context_menu->addSeparator();
    m_pictures_context_menu->addAction(ui->actionDownload);
    m_pictures_context_menu->addAction(ui->actionDiscardSelectedFiles);
    m_pictures_context_menu->addSeparator();
    m_pictures_context_menu->addAction(ui->actionDownloadAllAndQuit);
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

    m_connector = QSharedPointer<Connector>(new Connector(connection, true));

    bool success;
    success = connect(m_connector.data(), SIGNAL(failure(Connector::FailureReason)), this, SLOT(connectionFailure(Connector::FailureReason)), Qt::QueuedConnection);
    Q_ASSERT(success);
    success = connect(m_connector.data(), SIGNAL(success(QSharedPointer<Server>)), this, SLOT(connected(QSharedPointer<Server>)));
    Q_ASSERT(success);

    m_connector.data()->go();

    this->show();
}

void MainWindow::connected(QSharedPointer<Server> server)
{
    m_server = server;

    bool success;
    success = connect(m_server.data(), SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SLOT(processMessage(QSharedPointer<IncomingMessage>)));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(socketDisconnected()), this, SLOT(connectionEnded()));
    Q_ASSERT(success);
    success = connect(m_server.data(), SIGNAL(progress(qint64,qint64,IncomingMessage*)), this, SLOT(showProgress(qint64,qint64,IncomingMessage*)));
    Q_ASSERT(success);

    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new DirectoryListingRequestMessage()));

    enableCorrectControls();
}

void MainWindow::connectionFailure(Connector::FailureReason reason)
{
    Q_UNUSED(reason);
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
            updateShadowPositions(full_update_msg->motor_states, full_update_msg->motor_positions);
            break;
        }
        case IncomingMessage::ErrorMessage:
        {
            ErrorMessage * err_msg = (ErrorMessage *) msg.data();
            qDebug() << "Error message: " << err_msg->message;
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
            changeMotorBounds(init_info_msg->motor_boundaries);
            break;
        }
        default:
            qDebug() << "wtf got a message " << msg.data()->type;
            Q_ASSERT(false);
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

void MainWindow::updateShadowPosition(ShadowSlider * slider, qint8 motor_state, qint64 motor_position)
{
    slider->setEnabled((bool)(motor_state & FullUpdateMessage::MotorIsInitialized));
    slider->setShadowPosition((int)motor_position);
}

void MainWindow::updateShadowPositions(QVector<qint8> motor_states, QVector<qint64> motor_positions)
{
    updateShadowPosition(ui->orbitSliderA, motor_states.at(0), motor_positions.at(0));
    updateShadowPosition(ui->orbitSliderB, motor_states.at(1), motor_positions.at(1));
    updateShadowPosition(ui->liftSliderZ, motor_states.at(4), motor_positions.at(4));

    ui->shadowMinimap->setEnabled((bool)(motor_states.at(2) & FullUpdateMessage::MotorIsInitialized) && (bool)(motor_states.at(3) & FullUpdateMessage::MotorIsInitialized));
    ui->shadowMinimap->setShadowPosition(QPoint((int)motor_positions.at(2), (int)motor_positions.at(3)));
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

    QDir downloadDir(m_connection_settings->download_directory);
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
    if (msg->type == IncomingMessage::FileDownloadResult)
        m_progressDialog->setValue((int)(m_bytes_done + bytes_done));
}

void MainWindow::enableCorrectControls()
{
    bool an_item_exists = ui->picturesList->count() > 0;
    ui->actionDownloadAllAndQuit->setEnabled(an_item_exists);
    ui->actionSelectAll->setEnabled(an_item_exists);

    bool an_item_is_selected = ui->picturesList->selectedItems().count() > 0;
    ui->actionDownload->setEnabled(an_item_is_selected);
    ui->actionDiscardSelectedFiles->setEnabled(an_item_is_selected);

    bool server_connected = ! m_server.isNull() && (m_server.data()->loginStatus() == ServerTypes::Success);
    ui->actionTakeSnapshot->setEnabled(server_connected);
    ui->snapshotButton->setEnabled(server_connected);
    ui->shadowMinimap->setEnabled(server_connected);
    ui->liftSliderZ->setEnabled(server_connected);
    ui->orbitSliderA->setEnabled(server_connected);
    ui->orbitSliderB->setEnabled(server_connected);
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
    QString result = QFileDialog::getExistingDirectory(this, QString(), m_connection_settings->download_directory);
    if (result.isEmpty())
        return;
    m_connection_settings->download_directory = result;
    Settings::save();
}

bool MainWindow::checkDownloadDirectory()
{
    // make sure we have a download dir
    if (! m_connection_settings->download_directory.isEmpty())
        return true;
    QString result = QFileDialog::getExistingDirectory(this);
    if (result.isEmpty())
        return false;
    m_connection_settings->download_directory = result;
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

void MainWindow::changeMotorBounds(QVector<InitInfoMessage::MotorBoundaries> bounds)
{
    ui->orbitSliderA->blockSignals(true);
    ui->orbitSliderA->setMinimum((int)bounds.at(0).min);
    ui->orbitSliderA->setMaximum((int)bounds.at(0).max);
    ui->orbitSliderA->blockSignals(false);

    ui->orbitSliderB->blockSignals(true);
    ui->orbitSliderB->setMinimum((int)bounds.at(1).min);
    ui->orbitSliderB->setMaximum((int)bounds.at(1).max);
    ui->orbitSliderB->blockSignals(false);

    ui->shadowMinimap->setMaxPosition(QPoint((int)bounds.at(2).max, (int)bounds.at(3).max));

    ui->liftSliderZ->blockSignals(true);
    ui->liftSliderZ->setMinimum(bounds.at(4).min);
    ui->liftSliderZ->setMaximum(bounds.at(4).max);
    ui->liftSliderZ->blockSignals(false);
}

void MainWindow::showEvent(QShowEvent *)
{
    // ignore failure
    this->restoreGeometry(Settings::main_window_geometry);
    this->restoreState(Settings::main_window_state);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    Settings::main_window_geometry = this->saveGeometry();
    Settings::main_window_state = this->saveState();
    Settings::save();
}

QString MainWindow::getNextDownloadFilename()
{
    forever {
        QDir folder(m_connection_settings->download_directory);
        QString filename = folder.absoluteFilePath(QString("img_") + QString::number(m_next_download_number) + QString(".jpg"));
        if (! QFileInfo(filename).exists())
            return filename;
        m_next_download_number += 1;
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
