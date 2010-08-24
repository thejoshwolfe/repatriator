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

        public MainWindow()
        {
            InitializeComponent();

            connectionManager = ConnectionManager.load();
            connectionManager.connectionUpdate += new Action<ConnectionStatus>(connectionManager_connectionUpdate);
            connectionManager.loginFinished += new Action<LoginStatus>(connectionManager_loginFinished);

            // auto connect if we're good to go
            maybeStartConnectionManager();
        }

        private void maybeStartConnectionManager()
        {
            bool goodToGo = connectionManager.hasValidSettings();
            statusLabel.Text = goodToGo ? "connecting" : "not connected";
            serverText.Enabled = !goodToGo;
            userNameText.Enabled = !goodToGo;
            connectButton.Enabled = !goodToGo;
            if (goodToGo)
                connectionManager.start();
        }
        private void updateStatus_safe(string message)
        {
            BeginInvoke(new Action(delegate()
            {
                statusLabel.Text = message;
            }));
        }

        private void connectionManager_connectionUpdate(ConnectionStatus status)
        {
            switch (status)
            {
                case ConnectionStatus.Trouble:
                    updateStatus_safe("connection trouble");
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
            connectionManager.setEverything(serverName, serverPort, userName);
            maybeStartConnectionManager();
        }
    }
}
