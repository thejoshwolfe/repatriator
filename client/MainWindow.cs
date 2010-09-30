using System;
using System.Drawing;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace repatriator_client
{
    public partial class MainWindow : Form
    {
        private ConnectionManager connectionManager;
        private int retryFailures = 0;

        public MainWindow()
        {
            InitializeComponent();

            connectionManager = ConnectionManager.load();
            connectionManager.connectionUpdate += new Action<ConnectionStatus>(connectionManager_connectionUpdate);
            connectionManager.loginFinished += new Action<LoginStatus>(connectionManager_loginFinished);

            butterflyElemtnHost.Child = new ButterflyControl();

            // auto connect if we're good to go
            maybeStartConnectionManager();
        }

        private void maybeStartConnectionManager()
        {
            bool goodToGo = connectionManager.hasValidSettings();
            updateConnectionWidgets(goodToGo);
            if (!goodToGo)
                return;
            retryFailures = 0;
            connectionManager.start();
        }
        private void updateConnectionWidgets(bool connecting)
        {
            updateStatus_safe(connecting ? "connecting" : "not connected");
            serverText.Enabled = !connecting;
            userNameText.Enabled = !connecting;
            passwordText.Enabled = !connecting;
            downloadDirectoryText.Enabled = !connecting;
            connectButton.Enabled = !connecting;
        }
        private void updateStatus_safe(string message)
        {
            Action action = new Action(delegate()
            {
                statusLabel.Text = message;
                Logging.debug(message);
            });
            if (InvokeRequired)
                BeginInvoke(action);
            else
                action();
        }

        private void connectionManager_connectionUpdate(ConnectionStatus status)
        {
            switch (status)
            {
                case ConnectionStatus.Trouble:
                    retryFailures++;
                    updateStatus_safe("connection trouble" + ".".Repeat(retryFailures));
                    break;
                case ConnectionStatus.Success:
                    updateStatus_safe("connected");
                    break;
            }
        }
        private void connectionManager_loginFinished(LoginStatus status)
        {
            switch (status)
            {
                case LoginStatus.ConnectionTrouble:
                    updateStatus_safe("connection trouble. i give up.");
                    break;
                case LoginStatus.ServerIsBogus:
                    updateStatus_safe("bad server specified");
                    break;
                case LoginStatus.LoginIsInvalid:
                    updateStatus_safe("invalid login");
                    break;
                case LoginStatus.Success:
                    updateStatus_safe("logged in");
                    break;
            }
            BeginInvoke(new Action(delegate()
            {
                if (status == LoginStatus.Success)
                {
                    setupPanel.Visible = false;
                }
                else
                {
                    updateConnectionWidgets(false);
                }
            }));
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            string serverString = serverText.Text;
            string[] nameAndPort = serverString.Split(":");
            if (nameAndPort.Length != 2)
                return;
            string serverName = nameAndPort[0];
            int serverPort;
            if (!int.TryParse(nameAndPort[1], out serverPort))
                return;
            string userName = userNameText.Text;
            string password = passwordText.Text;
            string downloadDirectory = downloadDirectoryText.Text;
            connectionManager.setEverything(serverName, serverPort, userName, password, downloadDirectory);
            maybeStartConnectionManager();
        }

        private void cancelConncetionButton_Click(object sender, EventArgs e)
        {
            connectionManager.cancel();
        }
    }
}
