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
    public partial class EditUserAccountWindow : Form
    {
        public class UserAccount
        {
            public string username;
            public string password;
            public bool is_admin;
        }

        private UserAccount user_account = null;

        public EditUserAccountWindow()
        {
            InitializeComponent();
        }

        public UserAccount showGetNewUser(IWin32Window owner)
        {
            usernameTextBox.Text = "";
            passwordTextBox.Text = "";
            adminCheckBox.Checked = false;
            usernameTextBox.Focus();

            this.Text = "New User Account";
            this.ShowDialog(owner);

            return user_account;
        }

        public UserAccount showEditUser(IWin32Window owner, UserAccount current_account)
        {
            usernameTextBox.Text = current_account.username;
            passwordTextBox.Text = current_account.password;
            adminCheckBox.Checked = current_account.is_admin;
            usernameTextBox.Focus();

            this.Text = "Edit User Account";
            this.ShowDialog(owner);

            return user_account;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            this.user_account = new UserAccount();
            this.user_account.username = usernameTextBox.Text;
            this.user_account.password = passwordTextBox.Text;
            this.user_account.is_admin = adminCheckBox.Checked;
            this.Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            this.user_account = null;
            this.Close();
        }
    }
}
