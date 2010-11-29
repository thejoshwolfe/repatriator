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
            EditUserAccountWindow.UserAccount new_account = (new EditUserAccountWindow()).showGetNewUser(this);
            if (new_account == null)
                return;
            DetailedUserInfo user = new DetailedUserInfo();
            user.username = new_account.username;
            user.password = new_account.password;
            user.permissions = new HashSet<Permission>();
            user.permissions.Add(Permission.OperateHardware);
            if (new_account.is_admin)
                user.permissions.Add(Permission.ManageUsers);
            user.changedStatus = DetailedUserInfo.ChangedStatus.New;

            users.Add(user.username, user);
            usersListBox.Items.Add(user.username);
            usersListBox.SelectedIndex = usersListBox.Items.Count - 1;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            // TODO don't do this on the GUI thread
            foreach (DetailedUserInfo user in users.Values)
            {
                switch (user.changedStatus)
                {
                    case DetailedUserInfo.ChangedStatus.New:
                        connectionManager.addUser(user.username, user.password, user.permissions);
                        break;
                    case DetailedUserInfo.ChangedStatus.Updated:
                        connectionManager.updateUser(user.username, user.password, user.permissions);
                        break;
                    case DetailedUserInfo.ChangedStatus.Deleted:
                        connectionManager.deleteUser(user.username);
                        break;
                }
            }
            Close();
        }

        private void AdminWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            connectionManager.close();
        }
    }
}

