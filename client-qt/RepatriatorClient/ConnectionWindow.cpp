#include "ConnectionWindow.h"
#include "ui_ConnectionWindow.h"

#include "EditConnectionWindow.h"
#include "AdminWindow.h"
#include "MainWindow.h"
#include "PasswordInputWindow.h"
#include "ConnectionSettings.h"
#include "Settings.h"

#include <QDebug>

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

void ConnectionWindow::on_buttonBox_rejected()
{
    this->close();
}

bool ConnectionWindow::oneConnectionIsSelected() const
{
    return ui->connectionTable->selectedItems().count() == 3;
}

void ConnectionWindow::on_connectionTable_itemSelectionChanged()
{
    enableCorrectControls();
}
