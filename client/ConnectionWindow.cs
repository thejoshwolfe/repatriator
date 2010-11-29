﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace repatriator_client
{
    public partial class ConnectionWindow : Form
    {
        public ConnectionWindow()
        {
            InitializeComponent();
        }

        private void newButton_Click(object sender, EventArgs e)
        {
            EditConnectionWindow editor = new EditConnectionWindow();
            ConnectionSettings new_conn = editor.showNew(this);
            if (new_conn == null)
                return;
            Settings.connections.Add(new_conn);
            Settings.save();
            refreshConnections();

            // select the new one
            connectionListView.Items[connectionListView.Items.Count - 1].Selected = true;
        }

        private void ConnectionWindow_Shown(object sender, EventArgs e)
        {
            refreshConnections();
            if (connectionListView.Items.Count > 0)
                connectionListView.Items[0].Selected = true;
        }

        private void refreshConnections()
        {
            // maybe perserve selection
            int selection = -1;
            foreach (int selectedIndex in connectionListView.SelectedIndices)
                selection = selectedIndex; // will only happen once.

            // load contents
            connectionListView.Items.Clear();
            foreach (ConnectionSettings connection in Settings.connections)
                connectionListView.Items.Add(new ListViewItem(new string[] { connection.url, connection.port.ToString(), connection.username }));

            // restore selection
            if (selection != -1 && connectionListView.Items.Count != 0)
            {
                if (selection >= connectionListView.Items.Count)
                    selection = connectionListView.Items.Count - 1;
                connectionListView.Items[selection].Selected = true;
            }

            enableCorrectControls();
        }

        private void editButton_Click(object sender, EventArgs e)
        {
            if (connectionListView.SelectedIndices.Count == 1)
            {
                ConnectionSettings conn = Settings.connections[connectionListView.SelectedIndices[0]];
                EditConnectionWindow editor = new EditConnectionWindow();
                editor.showEdit(this, conn);
                Settings.save();
            }
            refreshConnections();
        }

        private void deleteButton_Click(object sender, EventArgs e)
        {
            if (connectionListView.SelectedIndices.Count == 1)
            {
                Settings.connections.RemoveAt(connectionListView.SelectedIndices[0]);
                Settings.save();
            }
            refreshConnections();
        }

        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {
            editButton_Click(sender, e);
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            deleteButton_Click(sender, e);
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            newButton_Click(sender, e);
        }

        private void adminButton_Click(object sender, EventArgs e)
        {
            if (connectionListView.SelectedIndices.Count != 1)
                return;

            ConnectionSettings conn = Settings.connections[connectionListView.SelectedIndices[0]];
            ConnectionManager connectionManager = establishConnection(conn, false);
            if (connectionManager == null)
                return;

            AdminWindow adminWindow = new AdminWindow(connectionManager);
            Hide();
            adminWindow.ShowDialog();
            Show();
        }
        private ConnectionManager establishConnection(ConnectionSettings conn, bool hardware)
        {
            string password = conn.password;
            if (password == "")
            {
                PasswordInputWindow inputPasswordWindow = new PasswordInputWindow("Authentication Required", "&Login");
                password = inputPasswordWindow.showGetPassword(this, password, conn.username);
                if (password == "")
                    return null;
            }
            ConnectionManager connectionManager = new ConnectionManager(conn, password, hardware);

            ConnectionStatusWindow statusWindow = new ConnectionStatusWindow(connectionManager, conn.username, hardware);
            statusWindow.ShowDialog(this);
            if (!statusWindow.result)
                return null;

            return connectionManager;
        }

        private void adminToolStripMenuItem_Click(object sender, EventArgs e)
        {
            adminButton_Click(sender, e);
        }

        private void connectionListView_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            loginButton_Click(sender, e);
        }

        private void loginButton_Click(object sender, EventArgs e)
        {
            if (connectionListView.SelectedIndices.Count != 1)
                return;
            ConnectionSettings conn = Settings.connections[connectionListView.SelectedIndices[0]];
            ConnectionManager connectionManager = establishConnection(conn, true);
            if (connectionManager == null)
                return;

            MainWindow mainWindow = new MainWindow(connectionManager);
            Hide();
            mainWindow.ShowDialog();
            Close();
        }

        private void enableCorrectControls()
        {
            adminButton.Enabled = editButton.Enabled = deleteButton.Enabled = loginButton.Enabled = (connectionListView.SelectedIndices.Count == 1);
        }

        private void connectionListView_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            enableCorrectControls();
        }

        private void ConnectionWindow_FormClosed(object sender, FormClosedEventArgs e)
        {
            Application.Exit();
        }
        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
