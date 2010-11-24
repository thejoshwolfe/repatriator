using System;
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

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void newButton_Click(object sender, EventArgs e)
        {
            EditConnectionWindow editor = new EditConnectionWindow();
            Connection new_conn = editor.showNew(this);
            if (new_conn != null)
            {
                Settings.connections.Add(new_conn);
                Settings.save();
            }
        }

        private void ConnectionWindow_Shown(object sender, EventArgs e)
        {
            connectionListView.Items.Clear();
            foreach (Connection connection in Settings.connections)
            {
                connectionListView.Items.Add(new ListViewItem(new string[]{connection.url, connection.port, connection.username}));
            }
            if (connectionListView.Items.Count > 0)
            {
                connectionListView.Items[0].Selected = true;
            }
            enableCorrectControls();
        }

        private void editButton_Click(object sender, EventArgs e)
        {
            if (connectionListView.SelectedIndices.Count == 1)
            {
                Connection conn = Settings.connections[connectionListView.SelectedIndices[0]];
                EditConnectionWindow editor = new EditConnectionWindow();
                editor.showEdit(this, conn);
                Settings.save();
            }
        }

        private void deleteButton_Click(object sender, EventArgs e)
        {
            if (connectionListView.SelectedIndices.Count == 1)
            {
                Settings.connections.RemoveAt(connectionListView.SelectedIndices[0]);
            }
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
            if (connectionListView.SelectedIndices.Count == 1)
            {
                Connection conn = Settings.connections[connectionListView.SelectedIndices[0]];
                // TODO: open connection to selected connection with the purpose of doing admin
            }
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
            if (connectionListView.SelectedIndices.Count == 1)
            {
                Connection conn = Settings.connections[connectionListView.SelectedIndices[0]];
                // TODO: open connection to selected connection with the purpose of using hardware
            }
        }

        private void enableCorrectControls()
        {
            adminButton.Enabled = editButton.Enabled = deleteButton.Enabled = loginButton.Enabled = (connectionListView.SelectedIndices.Count == 1);
        }

        private void connectionListView_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            enableCorrectControls();
        }
    }
}
