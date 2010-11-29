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
    public partial class ConnectionStatusWindow : Form
    {
        private readonly ConnectionManager connectionManager;
        private bool hardware;
        private int connectionTroubleCount;
        public bool result = false;
        private string username;
        public ConnectionStatusWindow(ConnectionManager connectionManager, string username, bool hardware)
        {
            this.connectionManager = connectionManager;
            this.username = username;
            this.hardware = hardware;
            InitializeComponent();

            connectionManager.connectionUpdate += new Action<ConnectionStatus>(connectionManager_connectionUpdate);
            connectionManager.loginFinished += new Action<LoginStatus>(connectionManager_loginFinished);

            beginAttempt();
        }
        private void beginAttempt()
        {
            connectionTroubleCount = 0;
            connectionManager.start();
        }

        private void connectionManager_loginFinished(LoginStatus status)
        {
            BeginInvoke(new Action(delegate()
            {
                switch (status)
                {
                    case LoginStatus.Success:
                        this.result = true;
                        Close();
                        break;
                    case LoginStatus.ConnectionTrouble:
                        switch (MessageBox.Show(this, "Unable to establish communication with server.", "Connection Trouble", MessageBoxButtons.RetryCancel, MessageBoxIcon.Warning))
                        {
                            case System.Windows.Forms.DialogResult.Retry:
                                beginAttempt();
                                break;
                            case System.Windows.Forms.DialogResult.Cancel:
                                Close();
                                break;
                        }
                        break;
                    case LoginStatus.InsufficientPrivileges:
                        MessageBox.Show(this, "You do not have permission to " +
                            (hardware ? "operate the hardware" : "manage users")
                            + ".", "Permission Denied", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        Close();
                        break;
                    case LoginStatus.LoginIsInvalid:
                        PasswordInputWindow window = new PasswordInputWindow("Invalid Login", "&Retry");
                        string newPassword = window.showGetPassword(this, "", username);
                        if (newPassword == "")
                        {
                            Close();
                            return;
                        }
                        connectionManager.password = newPassword;
                        beginAttempt();
                        break;
                    case LoginStatus.ServerIsBogus:
                        MessageBox.Show(this, "Invalid server specified.", "Invalid Server", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        Close();
                        break;
                }
            }));
        }

        private void connectionManager_connectionUpdate(ConnectionStatus status)
        {
            switch (status)
            {
                case ConnectionStatus.Trouble:
                    connectionTroubleCount++;
                    BeginInvoke(new Action(delegate()
                    {
                        statusLabel.Text = "connection trouble" + ".".Repeat(connectionTroubleCount);
                    }));
                    break;
            }
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            connectionManager.cancel();
            Close();
        }
    }
}
