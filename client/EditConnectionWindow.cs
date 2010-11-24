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
    public partial class EditConnectionWindow : Form
    {
        private enum Mode
        {
            NewMode,
            EditMode,
        }

        private Mode mode;

        private Connection connection;

        private const string static_password = "12345678";
           
        public EditConnectionWindow()
        {
            InitializeComponent();
        }

        public Connection showNew(IWin32Window owner)
        {
            this.Text = "New Connection - Repatriator";
            mode = Mode.NewMode;

            // clear controls
            urlTextBox.Text = "";
            portTextBox.Text = "";
            userNameTextBox.Text = "";
            savePasswordCheckBox.Checked = false;
            passwordTextBox.Text = "";

            enableCorrectControls();

            connection = new Connection();
            this.ShowDialog(owner);
            return connection;
        }

        public Connection showEdit(IWin32Window owner, Connection conn)
        {
            this.Text = "Edit Connection - Repatriator";
            mode = Mode.EditMode;

            // load values from connection
            urlTextBox.Text = conn.url;
            portTextBox.Text = conn.port.ToString();
            userNameTextBox.Text = conn.username;
            bool save_password = conn.password.Length != 0;
            savePasswordCheckBox.Checked = save_password;
            passwordTextBox.Text = save_password ? static_password : "";

            enableCorrectControls();

            connection = conn;
            this.ShowDialog(owner);

            return connection;
        }

        private void updateConnectionWithControls()
        {
            connection.url = urlTextBox.Text;
            connection.port = 0;
            int.TryParse(portTextBox.Text, out connection.port);
            connection.username = userNameTextBox.Text;
            connection.password = (savePasswordCheckBox.Checked && passwordTextBox.Text != static_password) ? passwordTextBox.Text : "";
        }

        private void enableCorrectControls()
        {
            passwordTextBox.Enabled = savePasswordCheckBox.Checked;
            passwordLabel.Enabled = savePasswordCheckBox.Checked;
        }

        private void savePasswordCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            enableCorrectControls();
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            updateConnectionWithControls();
            this.Close();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            connection = null;
            this.Close();
        }

        private void passwordTextBox_Enter(object sender, EventArgs e)
        {
            if (passwordTextBox.Text == static_password)
                passwordTextBox.Text = "";
        }

        private void portTextBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(('0' <= e.KeyChar && e.KeyChar <= '9') || e.KeyChar == '\b'))
                e.Handled = true;
        }
    }
}
