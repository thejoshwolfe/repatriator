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
    public partial class AdminWindow : Form
    {
        private readonly ConnectionManager connectionManager;
        private Dictionary<string, UserInfo> users = new Dictionary<string, UserInfo>();
        public AdminWindow(ConnectionManager connectionManager)
        {
            this.connectionManager = connectionManager;

            InitializeComponent();

            connectionManager.usersUpdated += new Action(connectionManager_usersUpdated);

            refreshList();
        }

        private void connectionManager_usersUpdated()
        {
            BeginInvoke(new Action(delegate()
            {
                usersListBox.Items.Clear();
                users.Clear();
                foreach (UserInfo user in connectionManager.users)
                {
                    usersListBox.Items.Add(user.username);
                    users.Add(user.username, user);
                }
                usersListBox.Enabled = true;
            }));
        }

        private void refreshList()
        {
            usersListBox.Items.Clear();
            usersListBox.Items.Add("updating...");
            usersListBox.Enabled = false;

            connectionManager.refreshUserList();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}

