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
        private Dictionary<string, DetailedUserInfo> users = new Dictionary<string, DetailedUserInfo>();
        public AdminWindow(ConnectionManager connectionManager)
        {
            this.connectionManager = connectionManager;

            InitializeComponent();

            connectionManager.usersUpdated += new Action(connectionManager_usersUpdated);

            usersListBox.Items.Add("updating...");
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
                    users.Add(user.username, new DetailedUserInfo(user));
                }
                usersListBox.Enabled = true;
                newButton.Enabled = true;
            }));
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

            if (userIsSelected)
            {
                // user is selected
                DetailedUserInfo user = users[username];
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

        private void newButton_Click(object sender, EventArgs e)
        {
            // TODO get this from a form:
            DetailedUserInfo user = new DetailedUserInfo();
            user.username = "newguy";
            user.password = "abcdefg";
            user.permissions = new HashSet<Permission>();
            user.permissions.Add(Permission.OperateHardware);
            user.changed = true;

            users.Add(user.username, user);
            usersListBox.Items.Add(user.username);
            usersListBox.SelectedIndex = usersListBox.Items.Count - 1;
        }
    }
}

