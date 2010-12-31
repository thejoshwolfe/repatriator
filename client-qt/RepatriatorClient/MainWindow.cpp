#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ConnectionWindow.h"
#include "Settings.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>

MainWindow * MainWindow::s_instance = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_progressDialog(new QProgressDialog(this, Qt::Dialog))
{
    m_progressDialog->setWindowModality(Qt::NonModal);
    m_progressDialog->setWindowTitle(tr("Receiving data"));
    m_progressDialog->setMinimumDuration(2000);
    ui->setupUi(this);

    this->setCentralWidget(ui->displayWidget);
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

    Connector * connector = Connector::create(connection, true);

    bool success;
    success = connect(connector, SIGNAL(failure(Connector::FailureReason)), this, SLOT(connectionFailure(Connector::FailureReason)), Qt::DirectConnection);
    Q_ASSERT(success);
    success = connect(connector, SIGNAL(success(QSharedPointer<Server>)), this, SLOT(connected(QSharedPointer<Server>)), Qt::DirectConnection);
    Q_ASSERT(success);

    connector->go();

    this->show();
}

void MainWindow::connected(QSharedPointer<Server> server)
{
    m_server = server;

    bool success;
    success = connect(m_server.data(), SIGNAL(messageReceived(QSharedPointer<IncomingMessage>)), this, SLOT(processMessage(QSharedPointer<IncomingMessage>)), Qt::DirectConnection);
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
    this->close();
    cleanup();
}

void MainWindow::processMessage(QSharedPointer<IncomingMessage> msg)
{
    switch (msg.data()->type) {
        case IncomingMessage::FullUpdate:
        {
            FullUpdateMessage * full_update_msg = (FullUpdateMessage *) msg.data();
            ui->displayWidget->prepareDisplayImage(full_update_msg->image);
            updateShadowPositions(full_update_msg->motor_positions);
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
    }
}

void MainWindow::updateShadowPositions(QVector<qint64> motor_positions)
{
//    orbitSliderA.ShadowPosition = (int)connectionManager.motorPositions[0];
//    orbitSliderB.ShadowPosition = (int)connectionManager.motorPositions[1];
//    shadowMinimap.ShadowPosition = new Point((int)connectionManager.motorPositions[2], (int)connectionManager.motorPositions[3]);
//    liftSliderZ.ShadowPosition = (int)connectionManager.motorPositions[4];
}

void MainWindow::saveFile(QByteArray blob, QString filename)
{
    QDir downloadDir(m_connection_settings->download_directory);
    if (! downloadDir.exists())
        return;
    QFile downloadFile(downloadDir.absoluteFilePath(filename));
    downloadFile.open(QFile::WriteOnly);
    downloadFile.write(blob);
    downloadFile.close();

    // successful download, discard server copy
    m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new FileDeleteRequestMessage(filename)));

    if (filename == m_quit_after_this_file)
        m_server.data()->finishWritingAndDisconnect();
}

void MainWindow::cleanup()
{
    m_server.clear();
}

void MainWindow::showProgress(qint64 bytes_done, qint64 bytes_total, IncomingMessage *msg)
{
    Q_UNUSED(msg);
    m_progressDialog->setMinimum(0);
    m_progressDialog->setMaximum((int)bytes_total);
    m_progressDialog->setValue((int)bytes_done);
}

void MainWindow::enableCorrectControls()
{
    bool an_item_is_selected = ui->picturesList->selectedItems().count() > 0;
    ui->actionDownload->setEnabled(an_item_is_selected);
    ui->actionDiscardSelectedFiles->setEnabled(an_item_is_selected);
    ui->actionDownloadAllAndQuit->setEnabled(ui->picturesList->count() > 0);

    bool server_connected = ! m_server.isNull() && (m_server.data()->loginStatus() == ServerTypes::Success);
    ui->actionTakeSnapshot->setEnabled(server_connected);
    ui->snapshotButton->setEnabled(server_connected);
    ui->shadowMinimap->setEnabled(server_connected);
    ui->liftSliderZ->setEnabled(server_connected);
    ui->orbit3D->setEnabled(server_connected);
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
        m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new FileDownloadRequestMessage(filename)));
    }
}

void MainWindow::on_actionDownloadAllAndQuit_triggered()
{
    // send download file messages for each file
    for (int i = 0; i < ui->picturesList->count(); i++) {
        QString filename = ui->picturesList->item(i)->text();
        m_server.data()->sendMessage(QSharedPointer<OutgoingMessage>(new FileDownloadRequestMessage(filename)));
        m_quit_after_this_file = filename;
    }
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
