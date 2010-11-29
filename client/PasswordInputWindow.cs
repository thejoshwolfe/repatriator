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
    public partial class PasswordInputWindow : Form
    {
        private string returnPassword;

        public PasswordInputWindow(string title, string okButtonText)
        {
            InitializeComponent();
            this.Text = title;
            loginButton.Text = okButtonText;
        }

        public string showGetPassword(IWin32Window owner, string password, string username)
        {
            usernameLabel.Text = username;
            passwordTextBox.Text = password;
            passwordTextBox.Focus();
            passwordTextBox.SelectAll();
            this.ShowDialog(owner);
            return returnPassword;
        }

        private void loginButton_Click(object sender, EventArgs e)
        {
            returnPassword = passwordTextBox.Text;
            Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            returnPassword = "";
            Close();
        }
    }
}
