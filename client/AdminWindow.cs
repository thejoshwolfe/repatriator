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
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void usersListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            string username = (string)usersListBox.SelectedItem;

            bool userIsSelected = username != null;
            changePasswordButton.Enabled = userIsSelected;
            deleteButton.Enabled = userIsSelected;
            adminCheckbox.Enabled = userIsSelected;
            newButton.Enabled = true;

            if (userIsSelected)
            {
                // user is selected
                UserInfo user = users[username];
                adminCheckbox.Checked = user.permissions.Contains(Permission.ManageUsers);
            }
            else
            {
                // no selection
                adminCheckbox.Checked = false;
            }
        }

        private void AdminWindow_Shown(object sender, EventArgs e)
        {
            connectionManager.refreshUserList();
        }
    }
}

