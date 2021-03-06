#include "ConnectionWindow.h"
#include "ui_ConnectionWindow.h"

#include "EditConnectionWindow.h"
#include "AdminWindow.h"
#include "MainWindow.h"
#include "PasswordInputWindow.h"
#include "Settings.h"
#include "ConnectionSettings.h"

#include <QDebug>
#include <QMessageBox>
#include <QUrl>

ConnectionWindow * ConnectionWindow::s_instance = NULL;

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);

    m_loginButton = new QPushButton(tr("&Login"), this);

    ui->buttonBox->clear();
    ui->buttonBox->addButton(m_loginButton, QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(QDialogButtonBox::Cancel);

    bool success;
    success = connect(this, SIGNAL(rejected()), this, SLOT(handleRejected()));
    Q_ASSERT(success);
    success = connect(this, SIGNAL(accepted()), this, SLOT(handleAccepted()));
    Q_ASSERT(success);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

void ConnectionWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConnectionWindow::showEvent(QShowEvent *)
{
    refreshConnections();

    this->restoreGeometry(Settings::connection_window_geometry);

    Q_ASSERT(ui->connectionTable->columnCount() == Settings::connection_column_width.count());
    for (int i = 0; i < Settings::connection_column_width.count(); i++) {
        if (Settings::connection_column_width.at(i) <= 0)
            ui->connectionTable->resizeColumnToContents(i);
        else
            ui->connectionTable->setColumnWidth(i, Settings::connection_column_width.at(i));
    }

    // select the most recent one
    if (ui->connectionTable->rowCount() > Settings::last_connection_index)
        ui->connectionTable->selectRow(Settings::last_connection_index);
    else if (ui->connectionTable->rowCount() > 0)
        ui->connectionTable->selectRow(0);

    enableCorrectControls();
}

ConnectionWindow * ConnectionWindow::instance()
{
    if (! s_instance)
        s_instance = new ConnectionWindow();
    return s_instance;
}

void ConnectionWindow::on_adminButton_clicked()
{
    bool one_selected = oneConnectionIsSelected();
    Q_ASSERT(one_selected);
    if (one_selected) {
        int selected_row = ui->connectionTable->selectedItems().at(0)->row();
        ConnectionSettings * conn = Settings::connections.at(selected_row);

        AdminWindow::instance()->showAdmin(conn);
    } else {
        refreshConnections();
    }
}

void ConnectionWindow::on_newButton_clicked()
{
    EditConnectionWindow * editor = EditConnectionWindow::instance();
    ConnectionSettings * new_conn = new ConnectionSettings();
    if (! editor->showNew(new_conn)) {
        delete new_conn;
        return;
    }
    Settings::connections.append(new_conn);
    Settings::save();

    refreshConnections();

    // select the new one
    ui->connectionTable->selectRow(ui->connectionTable->rowCount() - 1);
    enableCorrectControls();
}

void ConnectionWindow::refreshConnections()
{
    // load contents
    ui->connectionTable->setRowCount(Settings::connections.count());
    for (int i = 0; i < Settings::connections.count(); i++) {
        QTableWidgetItem * hostItem = new QTableWidgetItem(Settings::connections.at(i)->host);
        ui->connectionTable->setItem(i, 0, hostItem);

        QTableWidgetItem * portItem = new QTableWidgetItem(QString::number(Settings::connections.at(i)->port));
        ui->connectionTable->setItem(i, 1, portItem);

        QTableWidgetItem * usernameItem = new QTableWidgetItem(Settings::connections.at(i)->username);
        ui->connectionTable->setItem(i, 2, usernameItem);
    }

    enableCorrectControls();
}

void ConnectionWindow::enableCorrectControls()
{
    bool enableButtons = oneConnectionIsSelected();
    ui->adminButton->setEnabled(enableButtons);
    ui->editButton->setEnabled(enableButtons);
    ui->deleteButton->setEnabled(enableButtons);
    m_loginButton->setEnabled(enableButtons);
}

void ConnectionWindow::on_editButton_clicked()
{
    bool one_selected = oneConnectionIsSelected();
    Q_ASSERT(one_selected);
    if (one_selected) {
        int selected_row = ui->connectionTable->selectedItems().at(0)->row();
        ConnectionSettings * conn = Settings::connections.at(selected_row);
        EditConnectionWindow * editor = EditConnectionWindow::instance();
        if (! editor->showEdit(conn))
            return;
        Settings::save();
    }
    refreshConnections();
}

void ConnectionWindow::on_deleteButton_clicked()
{
    bool one_selected = oneConnectionIsSelected();
    Q_ASSERT(one_selected);
    if (one_selected) {
        int selected_row = ui->connectionTable->selectedItems().at(0)->row();
        Settings::connections.removeAt(selected_row);
        Settings::save();
    }
    refreshConnections();
}

void ConnectionWindow::on_connectionTable_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    on_buttonBox_accepted();
}

void ConnectionWindow::on_buttonBox_accepted()
{
    this->accept();
}

void ConnectionWindow::hideEvent(QHideEvent *)
{
    // save window geometry and such
    Settings::connection_window_geometry = this->saveGeometry();

    // column widths
    Q_ASSERT(ui->connectionTable->columnCount() == Settings::connection_column_width.count());
    for (int i = 0; i < Settings::connection_column_width.count(); i++)
        Settings::connection_column_width.replace(i, ui->connectionTable->columnWidth(i));

    Settings::save();
}

void ConnectionWindow::on_buttonBox_rejected()
{
    this->reject();
}

bool ConnectionWindow::oneConnectionIsSelected() const
{
    return ui->connectionTable->selectedItems().count() == 3;
}

void ConnectionWindow::on_connectionTable_itemSelectionChanged()
{
    enableCorrectControls();
}

void ConnectionWindow::handleRejected()
{
    this->close();
}

void ConnectionWindow::handleAccepted()
{
    bool one_selected = oneConnectionIsSelected();
    Q_ASSERT(one_selected);
    if (one_selected) {
        int selected_row = ui->connectionTable->selectedItems().at(0)->row();

        // remember to auto-select next time
        Settings::last_connection_index = selected_row;
        Settings::save();

        ConnectionSettings * conn = Settings::connections.at(selected_row);

        this->hide();
        MainWindow::instance()->showWithConnection(conn);
    } else {
        refreshConnections();
    }
}

void ConnectionWindow::handleUrl(QString url_string)
{
    QUrl url(url_string);

    if (! url.isValid() || url.scheme() != "repatriator") {
        QMessageBox::warning(this, tr("Invalid URL"), tr("Unable to connect to %1: Invalid URL.").arg(url_string));
        return;
    }

    ConnectionSettings * new_conn = new ConnectionSettings;

    new_conn->host = url.host();
    new_conn->password = url.password();
    new_conn->port = url.port(57051);
    new_conn->username = url.userName();

    // if the url already exists, try to connect to the first saved one.
    // if a username is specified, it must be the same username.
    for (int i = 0; i < Settings::connections.count(); i++) {
        ConnectionSettings * conn = Settings::connections.at(i);
        if (conn->host.compare(new_conn->host, Qt::CaseInsensitive) == 0 && (new_conn->username.isEmpty() || conn->username == new_conn->username)) {
            // use this connection
            ui->connectionTable->selectRow(i);
            this->accept();
            delete new_conn;
            return;
        }
    }

    // otherwise create a new connection with this information, save it, and connect.
    Settings::connections.append(new_conn);
    Settings::save();

    refreshConnections();
    ui->connectionTable->selectRow(Settings::connections.count()-1);
    this->accept();
}

