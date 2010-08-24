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
            connectionManager.connectionFailure += connectionManager_connectionFailure;
            connectionManager.connectionSuccess += new Action(connectionManager_connectionSuccess);
            connectionManager.hostIsBogus += new Action(connectionManager_hostIsBogus);
            connectionManager.loginFailure += new Action(connectionManager_loginFailure);
            connectionManager.loginSuccess += new Action(connectionManager_loginSuccess);

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

        private void connectionManager_connectionFailure()
        {
            updateStatus_safe("connection trouble");
        }
        private void connectionManager_connectionSuccess()
        {
            updateStatus_safe("connected");
        }
        private void connectionManager_hostIsBogus()
        {
            updateStatus_safe("bad server specified");
        }
        private void connectionManager_loginFailure()
        {
            updateStatus_safe("invalid login");
        }
        private void connectionManager_loginSuccess()
        {
            updateStatus_safe("logged in");
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
